////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "Common.h"
#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "GuildMgr.h"
#include "GitRevision.h"
#include "World.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "DatabaseEnv.h"
#include "Arena.h"
#include "Chat.h"
#include "Group.h"
#include "Language.h"
#include "Log.h"
#include "Opcodes.h"
#include "Player.h"
#include "SharedDefines.h"
#include "SocialMgr.h"
#include "UpdateMask.h"
#include "Util.h"
#include "ScriptMgr.h"
#include "Battleground.h"
#include "AccountMgr.h"
#include "LFGMgr.h"
#include "POIMgr.hpp"
#include "ContributionMgr.hpp"
#include "InstanceScript.h"
#include "AuthenticationPackets.h"
#include "CharacterPackets.h"
#include "RecruitAFriendMgr.hpp"
#include "Challenge.h"

#ifndef CROSS
# include "Guild.h"
# include "GuildFinderMgr.h"
# include "PlayerDump.h"
#else
# include <time.h>
# include "InterRealmMgr.h"
#endif

bool LoginDBQueryHolder::Initialize()
{
    SetSize(MAX_PLAYER_LOGINDB_QUERY);

    bool l_Result = true;
    PreparedStatement* l_Statement = nullptr;

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_CHARACTER_SPELL);
    l_Statement->setUInt32(0, m_AccountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGINGB_SPELL, l_Statement);

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_HEIRLOOM_COLLECTION);
    l_Statement->setUInt32(0, m_AccountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGINDB_HEIRLOOM_COLLECTION, l_Statement);

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_TOYS);
    l_Statement->setUInt32(0, m_AccountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGINDB_TOYS, l_Statement);

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_ACHIEVEMENTS);
    l_Statement->setUInt32(0, m_AccountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGINDB_QUERY_LOAD_ACCOUNT_ACHIEVEMENTS, l_Statement);

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_CRITERIAPROGRESS);
    l_Statement->setUInt32(0, m_AccountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGINDB_QUERY_LOAD_CRITERIA_PROGRESS, l_Statement);

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_COSMETIC);
    l_Statement->setUInt32(0, m_AccountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGINDB_QUERY_LOAD_COSMETICS, l_Statement);

    l_Statement = LoginDatabase.GetPreparedStatement(LOGIN_SEL_ACCOUNT_QUEST_REWARDED);
    l_Statement->setUInt32(0, m_AccountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGINDB_QUERY_ACCOUNT_QUEST_REWARDED, l_Statement);

    return l_Result;
}

bool LoginQueryHolder::Initialize()
{
    SetSize(MAX_PLAYER_LOGIN_QUERY);

    bool l_Result = true;
    uint32 l_LowGuid = GUID_LOPART(m_guid);
    PreparedStatement* l_Statement = nullptr;

#ifdef CROSS
    InterRealmDatabasePool& l_RealmDatabase = *sInterRealmMgr->GetClientByRealmNumber(m_realmId)->GetDatabase();
#else
    auto& l_RealmDatabase = CharacterDatabase;
#endif

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADFROM, l_Statement);

#ifndef CROSS
    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GROUP_MEMBER);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADGROUP, l_Statement);
#endif

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_BOSS_LOOTED);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_BOSS_LOOTED, l_Statement);

#ifndef CROSS
    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_INSTANCE);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADBOUNDINSTANCES, l_Statement);
#endif /* CROSS */

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_AURAS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADAURAS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_AURAS_EFFECTS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADAURAS_EFFECTS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SPELL);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_CHAR_LOADSPELLS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_QUESTSTATUS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADQUESTSTATUS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_QUEST_OBJECTIVE_STATUS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_QUEST_OBJECTIVE_STATUS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_DAILYQUESTSTATUS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADDAILYQUESTSTATUS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_WEEKLYQUESTSTATUS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADWEEKLYQUESTSTATUS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_MONTHLYQUESTSTATUS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_MONTHLY_QUEST_STATUS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SEASONALQUESTSTATUS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADSEASONALQUESTSTATUS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_REPUTATION);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADREPUTATION, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_INVENTORY);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADINVENTORY, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_ITEMS_WITHOUT_INVENTORY);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Statement->setUInt32(1, l_LowGuid);
    l_Statement->setUInt32(2, l_LowGuid);
    l_Statement->setUInt32(3, l_LowGuid);
    l_Statement->setUInt32(4, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_ITEM_WITHOUT_INVENTORY, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHAR_VOID_STORAGE);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADVOIDSTORAGE, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_ACTIONS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADACTIONS, l_Statement);

#ifndef CROSS
    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_MAILCOUNT);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Statement->setUInt64(1, uint64(time(NULL)));
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADMAILCOUNT, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_MAIL);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADMAIL, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_MAILITEMS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADMAIL_ITEMS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SOCIALLIST);
    l_Statement->setUInt32(0, m_accountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADSOCIALLIST, l_Statement);
#endif

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_HOMEBIND);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADHOMEBIND, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SPELLCOOLDOWNS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADSPELLCOOLDOWNS, l_Statement);

    if (sWorld->getBoolConfig(CONFIG_DECLINED_NAMES_USED))
    {
        l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_DECLINEDNAMES);
        l_Statement->setUInt32(0, l_LowGuid);
        l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADDECLINEDNAMES, l_Statement);
    }

#ifndef CROSS
    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MEMBER);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADGUILD, l_Statement);
#endif

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_ACHIEVEMENTS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_ACHIEVEMENTS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_CRITERIAPROGRESS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_CRITERIA_PROGRESS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_EQUIPMENTSETS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADEQUIPMENTSETS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_BRACKET_DATA);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADBRACKETDATA, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_BGDATA);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADBGDATA, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_TALENTS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADTALENTS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_PLAYER_ACCOUNT_DATA);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADACCOUNTDATA, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_SKILLS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADSKILLS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_PVP_INFO);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADPVPINFO, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_BANNED);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADBANNED, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_QUESTSTATUSREW);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADQUESTSTATUSREW, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_INSTANCELOCKTIMES);
    l_Statement->setUInt32(0, m_accountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADINSTANCELOCKTIMES, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_PLAYER_CURRENCY);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADCURRENCY, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_PLAYER_CURRENCY_CAPS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADCURRENCYCAPS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHAR_ARCHAEOLOGY);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_ARCHAEOLOGY, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHAR_ARCHAEOLOGY_PROJECTS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_ARCHAEOLOGY_PROJECTS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHAR_ARCHAEOLOGY_SITES);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_ARCHAEOLOGY_SITES, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CUF_PROFILE);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_CUF_PROFILES, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_CHARGES_COOLDOWN);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_CHARGES_COOLDOWNS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_FOLLOWER);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_FOLLOWERS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_MISSION);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_MISSIONS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_WORKORDER);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_WORKORDERS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_TALENT);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_TALENTS, l_Statement);

    //////////////////////////////////////////////////////////////////////////

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_DRAENOR);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_DRAENOR, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_DRAENOR_BUILDING);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_DRAENOR_BUILDINGS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_DRAENOR_DAILY_TAVERN_DATA_CHAR);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_DRAENOR_MISSIONS_TAVERNDATA, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_DRAENOR_WEEKLY_TAVERN_DATA_CHAR);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_DRAENOR_WEEKLY_TAVERNDATA, l_Statement);

    //////////////////////////////////////////////////////////////////////////

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_GARRISON_BROKEN_ISLES);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_GARRISON_BROKEN_ISLES, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_DAILY_LOOT_COOLDOWNS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_DAILY_LOOT_COOLDOWNS, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_SEL_WORLD_STATES);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_WORLD_STATES, l_Statement);

#ifndef CROSS
    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_LOAD_BOUTIQUE_ITEM);
    l_Statement->setInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_BOUTIQUE_ITEM, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_LOAD_STORE_PROFESSION);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_STORE_PROFESSION, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_LOAD_BOUTIQUE_GOLD);
    l_Statement->setInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_BOUTIQUE_GOLD, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_LOAD_BOUTIQUE_TITLE);
    l_Statement->setInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_BOUTIQUE_TITLE, l_Statement);

    l_Statement = l_RealmDatabase.GetPreparedStatement(CHAR_LOAD_BOUTIQUE_LEVEL);
    l_Statement->setInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_BOUTIQUE_LEVEL, l_Statement);
#endif

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_LOAD_ARTIFACTS);
    l_Statement->setInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_ARTIFACTS, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_LOAD_ARTIFACT_POWERS);
    l_Statement->setInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_ARTIFACT_POWERS, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_LOAD_PVP_TALENTS);
    l_Statement->setInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_PVP_TALENTS, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_GLYPHS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_GLYPHS, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_WORLD_QUEST_REWARDS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_WORLD_QUEST_REWARDS, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PERSONAL_INSTANCE_LOCKOUTS);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_PERSONAL_INSTANCES_LOCKOUT, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_LEGENDARIES);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_LEGENDARIES, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_NOMI_RECIPES);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_NOMI_RECIPES, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PLAYERCHOICE_RESPONSE_REWARDED);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_PLAYERCHOICE_RESPONSE_REWARDED, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_REP_MODS);
    l_Statement->setUInt32(0, m_accountId);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOADREPUTATIONMODS, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_PVP_SEASON_REWARD_RANKING);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_PVP_SEASON_REWARD_RANKING, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHEST_TIMED_LOCKOUT);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_CHEST_TIMED_LOCKOUT, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CORPSE_LOCATION);
    l_Statement->setUInt64(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_CORPSE_LOCATION, l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_OBJECT_LOCKOUT);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Result &= SetPreparedQuery(PLAYER_LOGIN_QUERY_LOAD_OBJECT_LOCKOUT, l_Statement);

    return l_Result;
}

#ifndef CROSS

uint8 gExpansionPerRace[] =
{
    MAX_EXPANSION,                      ///< RACE_NONE
    EXPANSION_VANILLA,                  ///< RACE_HUMAN
    EXPANSION_VANILLA,                  ///< RACE_ORC
    EXPANSION_VANILLA,                  ///< RACE_DWARF
    EXPANSION_VANILLA,                  ///< RACE_NIGHTELF
    EXPANSION_VANILLA,                  ///< RACE_UNDEAD_PLAYER
    EXPANSION_VANILLA,                  ///< RACE_TAUREN
    EXPANSION_VANILLA,                  ///< RACE_GNOME
    EXPANSION_VANILLA,                  ///< RACE_TROLL
    EXPANSION_CATACLYSM,                ///< RACE_GOBLIN
    EXPANSION_THE_BURNING_CRUSADE,      ///< RACE_BLOODELF
    EXPANSION_THE_BURNING_CRUSADE,      ///< RACE_DRAENEI
    MAX_EXPANSION,                      ///< RACE_FEL_ORC
    MAX_EXPANSION,                      ///< RACE_NAGA
    MAX_EXPANSION,                      ///< RACE_BROKEN
    MAX_EXPANSION,                      ///< RACE_SKELETON
    MAX_EXPANSION,                      ///< RACE_VRYKUL
    MAX_EXPANSION,                      ///< RACE_TUSKARR
    MAX_EXPANSION,                      ///< RACE_FOREST_TROLL
    MAX_EXPANSION,                      ///< RACE_TAUNKA
    MAX_EXPANSION,                      ///< RACE_NORTHREND_SKELETON
    MAX_EXPANSION,                      ///< RACE_ICE_TROLL
    EXPANSION_CATACLYSM,                ///< RACE_WORGEN
    MAX_EXPANSION,                      ///< RACE_GILNEAN
    EXPANSION_MISTS_OF_PANDARIA,        ///< RACE_PANDAREN_NEUTRAL
    EXPANSION_MISTS_OF_PANDARIA,        ///< RACE_PANDAREN_ALLIANCE
    EXPANSION_MISTS_OF_PANDARIA,        ///< RACE_PANDAREN_HORDE
    EXPANSION_LEGION,                   ///< RACE_NIGHTBORNE
    EXPANSION_LEGION,                   ///< RACE_HIGHMOUNTAIN_TAUREN
    EXPANSION_LEGION,                   ///< RACE_VOID_ELF
    EXPANSION_LEGION                    ///< RACE_LIGHTFORGED_DRAENEI
};

void WorldSession::HandleCharEnum(PreparedQueryResult p_Result)
{
    uint32 l_RealmRaceCount = 0;

    uint8 l_DemonHunterCount = 0;
    bool l_CanAlwaysCreateDemonHunter = false;
    bool l_HasLevel70 = false;
    bool l_HasDemonHunter = false;
    bool l_IsDemonHunterCreationAllowed = false;
    bool l_IsDeletedCharacters = false;
    bool l_IsAlliedRacesCreationAllowed = true;

    uint32 l_CharacterCount             = p_Result ? uint32(p_Result->GetRowCount()) : 0;

    bool l_CanCreateCharacter     = true;
    bool l_HasDisabledClassesMask = false;


    for (uint32 l_I = 0; l_I < sizeof(gExpansionPerRace); ++l_I)
    {
        if (gExpansionPerRace[l_I] != MAX_EXPANSION)
            ++l_RealmRaceCount;
    }

    WorldPacket l_Data(SMSG_ENUM_CHARACTERS_RESULT, 5 * 1024);
    l_Data.WriteBit(l_CanCreateCharacter);          ///< Allow char creation
    l_Data.WriteBit(0);                             ///< IsDeletedCharacters

    size_t l_IsDemonHunterCreationAllowedPos = l_Data.bitwpos();
    l_Data.WriteBit(l_IsDemonHunterCreationAllowed);///<

    size_t l_HasDemonHunterPos = l_Data.bitwpos();
    l_Data.WriteBit(l_HasDemonHunter);              ///<

    l_Data.WriteBit(false);                            ///< Unk7x
    l_Data.WriteBit(l_HasDisabledClassesMask);         ///< HasDisabledClassesMask
    l_Data.WriteBit(l_IsAlliedRacesCreationAllowed);   ///< IsAlliedRacesCreationAllowed
    l_Data.FlushBits();

    l_Data << uint32(l_CharacterCount);                ///< Account character count

    size_t l_MaxCharLevelPos = l_Data.wpos();
    uint32 l_MaxCharLevel = 0;
    l_Data << uint32(l_MaxCharLevel);

    l_Data << uint32(l_RealmRaceCount);

    if (l_HasDisabledClassesMask)
        l_Data << uint32(0);                        ///< DisabledClassesMask

    if (p_Result)
    {
        do
        {
            uint32 l_GuidLow = (*p_Result)[0].GetUInt32();

            Player::BuildEnumData(p_Result, &l_Data);

            /// This can happen if characters are inserted into the database manually. Core hasn't loaded name data yet.

            uint8 l_Class = (*p_Result)[3].GetUInt8();
            uint8 l_Level = (*p_Result)[7].GetUInt8();
            uint8 l_LoginFlags = (*p_Result)[15].GetUInt8();
            if (!sWorld->HasCharacterInfo(l_GuidLow))
                sWorld->AddCharacterInfo(l_GuidLow, (*p_Result)[1].GetString(), GetAccountId(), (*p_Result)[4].GetUInt8(), (*p_Result)[2].GetUInt8(), l_Class, l_Level, l_LoginFlags);

            if (l_Class == CLASS_DEMON_HUNTER)
                l_DemonHunterCount++;

            if (l_DemonHunterCount >= 1)
                l_HasDemonHunter = false;

            if (l_Level >= 70 || l_CanAlwaysCreateDemonHunter)
                l_HasLevel70 = true;

            l_MaxCharLevel = std::max(l_MaxCharLevel, (uint32)l_Level);

            l_IsDemonHunterCreationAllowed = (l_HasLevel70 && !l_HasDemonHunter) || l_CanAlwaysCreateDemonHunter;

        } while (p_Result->NextRow());
    }

    for (uint32 l_I = 0; l_I < sizeof(gExpansionPerRace); ++l_I)
    {
        if (gExpansionPerRace[l_I] != MAX_EXPANSION)
        {
            l_Data << uint32(l_I);                      ///< RaceID
            l_Data.WriteBit(1);                         ///< HasExpansion
            l_Data.WriteBit(1);                         ///< HasAchievement
            l_Data.WriteBit(1);                         ///< HasHeritageArmor
            l_Data.FlushBits();
        }
    }

    l_Data.PutBits(l_IsDemonHunterCreationAllowedPos, l_IsDemonHunterCreationAllowed, 1);
    l_Data.PutBits(l_HasDemonHunterPos, l_HasDemonHunter, 1);
    l_Data.put<uint32>(l_MaxCharLevelPos, l_MaxCharLevel);

    SendPacket(&l_Data);

    /// Update realm character count
    SQLTransaction trans = LoginDatabase.BeginTransaction();

    PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_REALM_CHARACTERS);
    stmt->setUInt32(0, l_CharacterCount);
    stmt->setUInt32(1, GetAccountId());
    stmt->setUInt32(2, g_RealmID);
    trans->Append(stmt);

    LoginDatabase.CommitTransaction(trans);
}

void WorldSession::HandleCharEnumOpcode(WorldPacket& /*recvData*/)
{
    // remove expired bans
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_EXPIRED_BANS);
    CharacterDatabase.Execute(stmt);

    /// get all the data necessary for loading all characters (along with their pets) on the account

    if (sWorld->getBoolConfig(CONFIG_DECLINED_NAMES_USED))
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ENUM_DECLINED_NAME);
    else
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ENUM);

    stmt->setUInt32(0, GetAccountId());

    m_CharEnumCallback = CharacterDatabase.AsyncQuery(stmt);
}

void WorldSession::HandleCharCreateOpcode(WorldPacket& p_RecvData)
{
    uint32 l_CharacterNameLenght    = 0;
    uint32 l_TemplateSetID          = 0;

    uint8 l_CharacterRace       = 0;
    uint8 l_CharacterClass      = 0;
    uint8 l_CharacterGender     = 0;
    uint8 l_CharacterSkin       = 0;
    uint8 l_CharacterFace       = 0;
    uint8 l_CharacterHairStyle  = 0;
    uint8 l_CharacterHairColor  = 0;
    uint8 l_CharacterFacialHair = 0;
    uint8 l_CharacterOutfitID   = 0;
    uint8 l_Tattoos             = 0;
    uint8 l_HornStyle           = 0;
    uint8 l_Blindfolds          = 0;

    std::string l_CharacterName;

    bool l_HaveTemplateSetID = false;

    //////////////////////////////////////////////////////////////////////////

    l_CharacterNameLenght   = p_RecvData.ReadBits(6);
    l_HaveTemplateSetID     = p_RecvData.ReadBit();

    p_RecvData >> l_CharacterRace;                                          ///< uint8
    p_RecvData >> l_CharacterClass;                                         ///< uint8
    p_RecvData >> l_CharacterGender;                                        ///< uint8
    p_RecvData >> l_CharacterSkin;                                          ///< uint8
    p_RecvData >> l_CharacterFace;                                          ///< uint8
    p_RecvData >> l_CharacterHairStyle;                                     ///< uint8
    p_RecvData >> l_CharacterHairColor;                                     ///< uint8
    p_RecvData >> l_CharacterFacialHair;                                    ///< uint8
    p_RecvData >> l_CharacterOutfitID;                                      ///< uint8
    p_RecvData >> l_Tattoos;
    p_RecvData >> l_HornStyle;
    p_RecvData >> l_Blindfolds;

    l_CharacterName = p_RecvData.ReadString(l_CharacterNameLenght);

    if (l_HaveTemplateSetID)
        p_RecvData >> l_TemplateSetID;

    //////////////////////////////////////////////////////////////////////////

    sLog->outAshran("Receive HandleCharCreateOpcode from %u", GetAccountId());

    WorldPacket l_CreationResponse(SMSG_CREATE_CHAR, 1);                    ///< returned with diff.values in all cases

    if (l_TemplateSetID)
    {
        bool l_TemplateAvailable = sWorld->getBoolConfig(CONFIG_TEMPLATES_ENABLED);
        CharacterTemplate const* l_Template = sObjectMgr->GetCharacterTemplate(l_TemplateSetID);

        if (!l_TemplateAvailable || !l_Template || l_Template->m_PlayerClass != l_CharacterClass)
        {
            l_CreationResponse << (uint8)CHAR_CREATE_ERROR;
            SendPacket(&l_CreationResponse);
            return;
        }
    }

    if (l_TemplateSetID && l_CharacterRace == RACE_PANDAREN_NEUTRAL)
    {
        l_CreationResponse << (uint8)CHAR_CREATE_CHARACTER_CHOOSE_RACE;
        SendPacket(&l_CreationResponse);
        return;
    }

    if (!l_TemplateSetID && (l_CharacterRace == RACE_PANDAREN_HORDE || l_CharacterRace == RACE_PANDAREN_ALLIANCE))
    {
        l_CreationResponse << (uint8)CHAR_CREATE_ERROR;
        SendPacket(&l_CreationResponse);
        return;
    }

    if (AccountMgr::IsPlayerAccount(GetSecurity()))
    {
        if (uint32 l_Mask = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_DISABLED))
        {
            bool l_IsDisabled = false;

            uint32 l_RaceTeam = Player::TeamForRace(l_CharacterRace);

            switch (l_RaceTeam)
            {
                case ALLIANCE:
                    l_IsDisabled = l_Mask & (1 << 0);
                    break;

                case HORDE:
                    l_IsDisabled = l_Mask & (1 << 1);
                    break;
            }

            if (l_IsDisabled)
            {
                l_CreationResponse << (uint8)CHAR_CREATE_DISABLED;
                SendPacket(&l_CreationResponse);

                return;
            }
        }
    }

    ChrClassesEntry const* l_ClassEntry = sChrClassesStore.LookupEntry(l_CharacterClass);
    if (!l_ClassEntry)
    {
        l_CreationResponse << (uint8)CHAR_CREATE_FAILED;
        SendPacket(&l_CreationResponse);

        sLog->outError(LOG_FILTER_NETWORKIO, "Class (%u) not found in DB2 while creating new char for account (ID: %u): wrong DB2 files or cheater?", l_CharacterClass, GetAccountId());

        return;
    }

    ChrRacesEntry const* l_RaceEntry = sChrRacesStore.LookupEntry(l_CharacterRace);
    if (!l_RaceEntry)
    {
        l_CreationResponse << (uint8)CHAR_CREATE_FAILED;
        SendPacket(&l_CreationResponse);

        sLog->outError(LOG_FILTER_NETWORKIO, "Race (%u) not found in DB2 while creating new char for account (ID: %u): wrong DB2 files or cheater?", l_CharacterRace, GetAccountId());

        return;
    }

    if (AccountMgr::IsPlayerAccount(GetSecurity()))
    {
        uint64 l_RaceMaskDisabled = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_DISABLED_RACEMASK);

        if ((1LL << (l_CharacterRace - 1)) & l_RaceMaskDisabled)
        {
            l_CreationResponse << uint8(CHAR_CREATE_DISABLED);
            SendPacket(&l_CreationResponse);

            return;
        }

        uint32 classMaskDisabled = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_DISABLED_CLASSMASK);

        if ((1 << (l_CharacterClass - 1)) & classMaskDisabled)
        {
            l_CreationResponse << uint8(CHAR_CREATE_DISABLED);
            SendPacket(&l_CreationResponse);

            return;
        }
    }

    /// prevent character creating with invalid name
    if (!normalizePlayerName(l_CharacterName))
    {
        l_CreationResponse << (uint8)CHAR_NAME_NO_NAME;
        SendPacket(&l_CreationResponse);

        sLog->outError(LOG_FILTER_NETWORKIO, "Account:[%d] but tried to Create character with empty [name] ", GetAccountId());

        return;
    }

    /// check name limitations
    uint8 l_Result = ObjectMgr::CheckPlayerName(l_CharacterName, true);
    if (l_Result != CHAR_NAME_SUCCESS)
    {
        l_CreationResponse << uint8(l_Result);
        SendPacket(&l_CreationResponse);

        return;
    }

    if (AccountMgr::IsPlayerAccount(GetSecurity()) && sObjectMgr->IsReservedName(l_CharacterName))
    {
        l_CreationResponse << (uint8)CHAR_NAME_RESERVED;
        SendPacket(&l_CreationResponse);

        return;
    }

    /// speedup check for heroic class disabled case
    uint32 l_RequiredLevelForHeroicDK = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_MIN_LEVEL_FOR_HEROIC_CHARACTER_DK);
    uint32 l_RequiredLevelForHeroicDH = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_MIN_LEVEL_FOR_HEROIC_CHARACTER_DH);
    if (AccountMgr::IsPlayerAccount(GetSecurity()))
    {
        if (l_CharacterClass == Classes::CLASS_DEATH_KNIGHT && l_RequiredLevelForHeroicDK > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        {
            l_CreationResponse << (uint8)CHAR_CREATE_LEVEL_REQUIREMENT;
            SendPacket(&l_CreationResponse);
            return;
        }
        else if (l_CharacterClass == Classes::CLASS_DEMON_HUNTER && l_RequiredLevelForHeroicDH > sWorld->getIntConfig(CONFIG_MAX_PLAYER_LEVEL))
        {
            l_CreationResponse << (uint8)CHAR_CREATE_LEVEL_REQUIREMENT_DEMON_HUNTER;
            SendPacket(&l_CreationResponse);
            return;
        }
    }

    delete _charCreateCallback.GetParam();  // Delete existing if any, to make the callback chain reset to stage 0

    _charCreateCallback.SetParam(new CharacterCreateInfo(l_CharacterName, l_CharacterRace, l_CharacterClass, l_CharacterGender, l_CharacterSkin, l_CharacterFace, l_CharacterHairStyle, l_CharacterHairColor, l_CharacterFacialHair, 1, l_Tattoos, l_HornStyle, l_Blindfolds, l_TemplateSetID, p_RecvData));

    PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHECK_NAME);
    l_Stmt->setString(0, l_CharacterName);
    l_Stmt->setString(1, l_CharacterName);

    _charCreateCallback.SetFutureResult(CharacterDatabase.AsyncQuery(l_Stmt));
}

void WorldSession::HandleCharCreateCallback(PreparedQueryResult result, CharacterCreateInfo* createInfo)
{
    /** This is a series of callbacks executed consecutively as a result from the database becomes available.
        This is much more efficient than synchronous requests on packet handler, and much less DoS prone.
        It also prevents data syncrhonisation errors.
    */
    switch (_charCreateCallback.GetStage())
    {
        case 0:
        {
            if (result)
            {
                WorldPacket data(SMSG_CREATE_CHAR, 1);
                data << uint8(CHAR_CREATE_NAME_IN_USE);
                SendPacket(&data);
                delete createInfo;
                _charCreateCallback.Reset();
                return;
            }

            ASSERT(_charCreateCallback.GetParam() == createInfo);

            PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_SUM_CHARS);
            stmt->setUInt32(0, GetAccountId());

            _charCreateCallback.FreeResult();
            _charCreateCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));
            _charCreateCallback.NextStage();
        }
        break;
        case 1:
        {
            if (result)
            {
                Field* fields = result->Fetch();
                createInfo->CharCount = uint8(fields[0].GetUInt64()); // SQL's COUNT() returns uint64 but it will always be less than uint8.Max

                if (createInfo->CharCount >= sWorld->getIntConfig(CONFIG_CHARACTERS_PER_REALM))
                {
                    WorldPacket data(SMSG_CREATE_CHAR, 1);
                    data << uint8(CHAR_CREATE_SERVER_LIMIT);
                    SendPacket(&data);
                    delete createInfo;
                    _charCreateCallback.Reset();
                    return;
                }
            }

            bool allowTwoSideAccounts = !sWorld->IsPvPRealm() || sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_ACCOUNTS) || !AccountMgr::IsPlayerAccount(GetSecurity());
            uint32 skipCinematics = sWorld->getIntConfig(CONFIG_SKIP_CINEMATICS);

            _charCreateCallback.FreeResult();

            bool isHeroicClass = createInfo->Class == Classes::CLASS_DEATH_KNIGHT || createInfo->Class == Classes::CLASS_DEMON_HUNTER;
            if (!allowTwoSideAccounts || skipCinematics == 1 || isHeroicClass)
            {
                PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_CREATE_INFO);
                stmt->setUInt32(0, GetAccountId());
                stmt->setUInt32(1, (skipCinematics == 1 || isHeroicClass) ? (sWorld->getIntConfig(CONFIG_CHARACTERS_PER_REALM) - 1) : 1);
                _charCreateCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));
                _charCreateCallback.NextStage();
                return;
            }

            _charCreateCallback.NextStage();
            HandleCharCreateCallback(PreparedQueryResult(NULL), createInfo);   // Will jump to case 2
        }
        break;
        case 2:
        {
            bool haveSameRace = false;

            uint32 heroicReqLevel = 0;

            switch (createInfo->Class)
            {
                case Classes::CLASS_DEATH_KNIGHT: heroicReqLevel = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_MIN_LEVEL_FOR_HEROIC_CHARACTER_DK); break;
                case Classes::CLASS_DEMON_HUNTER: heroicReqLevel = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_MIN_LEVEL_FOR_HEROIC_CHARACTER_DH); break;
                default: heroicReqLevel = 0; break;
            }

            bool heroicReqLevelCheckPassed = (heroicReqLevel == 0);
            bool allowTwoSideAccounts = !sWorld->IsPvPRealm() || sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_ACCOUNTS) || !AccountMgr::IsPlayerAccount(GetSecurity());
            uint32 skipCinematics = sWorld->getIntConfig(CONFIG_SKIP_CINEMATICS);

            if (result)
            {
                /// Crashfix attempt
                if (!result->GetRowCount())
                {
                    sLog->outExtChat("#jarvis", "danger", true, "LEGION: WorldSession::HandleCharCreateCallback !result->GetRowCount()");
                    delete createInfo;
                    _charCreateCallback.Reset();
                    return;
                }

                /// Crashfix attempt
                if (result->GetFieldCount() != 3)
                {
                    sLog->outExtChat("#jarvis", "danger", true, "LEGION: WorldSession::HandleCharCreateCallback result->GetFieldCount() != 3");
                    delete createInfo;
                    _charCreateCallback.Reset();
                    return;
                }

                uint32 team = Player::TeamForRace(createInfo->Race);

                Field* field = result->Fetch();
                uint8 accRace  = field[1].GetUInt8();

                if (AccountMgr::IsPlayerAccount(GetSecurity()) && heroicReqLevel != 0)
                {
                    if (!heroicReqLevelCheckPassed)
                    {
                        uint8 accLevel = field[0].GetUInt8();
                        if (accLevel >= heroicReqLevel)
                            heroicReqLevelCheckPassed = true;
                    }
                }

                // need to check team only for first character
                // TODO: what to if account already has characters of both races?
                if (!allowTwoSideAccounts)
                {
                    uint32 accTeam = 0;
                    if (accRace > 0)
                        accTeam = Player::TeamForRace(accRace);

                    if (accTeam != team)
                    {
                        WorldPacket data(SMSG_CREATE_CHAR, 1);
                        data << uint8(CHAR_CREATE_PVP_TEAMS_VIOLATION);
                        SendPacket(&data);
                        delete createInfo;
                        _charCreateCallback.Reset();
                        return;
                    }
                }

                // search same race for cinematic or same class if need
                // TODO: check if cinematic already shown? (already logged in?; cinematic field)
                while ((skipCinematics == 1 && !haveSameRace) || heroicReqLevel != 0)
                {
                    if (!result->NextRow())
                        break;

                    field = result->Fetch();
                    accRace = field[1].GetUInt8();

                    if (!haveSameRace)
                        haveSameRace = createInfo->Race == accRace;

                    if (AccountMgr::IsPlayerAccount(GetSecurity()) && heroicReqLevel != 0)
                    {
                        if (!heroicReqLevelCheckPassed)
                        {
                            uint8 acc_level = field[0].GetUInt8();
                            if (acc_level >= heroicReqLevel)
                                heroicReqLevelCheckPassed = true;
                        }
                    }
                }
            }

            if (AccountMgr::IsPlayerAccount(GetSecurity()) && heroicReqLevel != 0 && !heroicReqLevelCheckPassed)
            {
                WorldPacket data(SMSG_CREATE_CHAR, 1);
                data << uint8(CHAR_CREATE_LEVEL_REQUIREMENT);
                SendPacket(&data);
                delete createInfo;
                _charCreateCallback.Reset();
                return;
            }

            if (createInfo->Data.rpos() < createInfo->Data.wpos())
            {
                uint8 unk;
                createInfo->Data >> unk;
                sLog->outDebug(LOG_FILTER_NETWORKIO, "Character creation %s (account %u) has unhandled tail data: [%u]", createInfo->Name.c_str(), GetAccountId(), unk);
            }

            Player newChar(this);
            newChar.GetMotionMaster()->Initialize();
            if (!newChar.Create(sObjectMgr->GenerateLowGuid(HIGHGUID_PLAYER), createInfo))
            {
                // Player not create (race/class/etc problem?)
                newChar.CleanupsBeforeDelete();

                WorldPacket data(SMSG_CREATE_CHAR, 1);
                data << uint8(CHAR_CREATE_ERROR);
                SendPacket(&data);
                delete createInfo;
                _charCreateCallback.Reset();
                return;
            }

            if ((haveSameRace && skipCinematics == 1) || skipCinematics == 2)
                newChar.setCinematic(1);                          // not show intro

            newChar.SetAtLoginFlag(AT_LOGIN_FIRST);               // First login


            // Player created, save it now
            uint32 l_AccountID = GetAccountId();

            newChar.SaveToDB(true, false, std::make_shared<MS::Utilities::Callback>([l_AccountID](bool p_Success) -> void
            {
                WorldSession* l_Session = sWorld->FindSession(l_AccountID);
                if (l_Session == nullptr)
                    return;

                WorldPacket l_Data(SMSG_CREATE_CHAR, 1);
                l_Data << uint8(p_Success ? CHAR_CREATE_SUCCESS : CHAR_CREATE_ERROR);
                l_Session->SendPacket(&l_Data);
            }));

            createInfo->CharCount++;

            SQLTransaction trans = LoginDatabase.BeginTransaction();

            PreparedStatement* stmt = LoginDatabase.GetPreparedStatement(LOGIN_UPD_REALM_CHARACTERS);
            stmt->setUInt32(0, createInfo->CharCount);
            stmt->setUInt32(1, GetAccountId());
            stmt->setUInt32(2, g_RealmID);
            trans->Append(stmt);

            LoginDatabase.CommitTransaction(trans);

            std::string IP_str = GetRemoteAddress();
            sLog->outInfo(LOG_FILTER_CHARACTER, "Account: %d (IP: %s) Create Character:[%s] (GUID: %u)", GetAccountId(), IP_str.c_str(), createInfo->Name.c_str(), newChar.GetGUIDLow());
            sScriptMgr->OnPlayerCreate(&newChar);
            sWorld->AddCharacterInfo(newChar.GetGUIDLow(), std::string(newChar.GetName()), GetAccountId(), newChar.getGender(), newChar.getRace(), newChar.getClass(), newChar.getLevel(), (uint32)AtLoginFlags::AT_LOGIN_NONE);

            newChar.CleanupsBeforeDelete();
            delete createInfo;
            _charCreateCallback.Reset();
        }
        break;
    }
}

void WorldSession::HandleCharDeleteOpcode(WorldPacket& recvData)
{
    uint64 charGuid;

    recvData.readPackGUID(charGuid);

    // can't delete loaded character
    if (ObjectAccessor::FindPlayer(charGuid))
        return;

    // is guild leader
    if (sGuildMgr->GetGuildByLeader(charGuid))
    {
        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << uint8(CHAR_DELETE_FAILED_GUILD_LEADER);
        SendPacket(&data);
        return;
    }

    if (m_muteTime && m_muteTime > time(NULL))
    {
        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << uint8(CHAR_DELETE_FAILED);
        SendPacket(&data);
        return;
    }

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_ACCOUNT_NAME_BY_GUID);
    stmt->setUInt32(0, GUID_LOPART(charGuid));

    uint32 l_AccountId = GetAccountId();
    CharacterDatabase.AsyncQuery(stmt, sWorld->GetQueryCallbackMgr(), [l_AccountId, charGuid](PreparedQueryResult const& p_Result)
    {
        if (!p_Result)
            return;

        uint32 accountId = 0;
        uint32 atLogin = 0;
        std::string name;

        Field* fields = p_Result->Fetch();
        accountId     = fields[0].GetUInt32();
        name          = fields[1].GetString();
        atLogin       = fields[2].GetUInt16();

        // prevent deleting other players' characters using cheating tools
        if (accountId != l_AccountId)
            return;

        WorldSession* l_Session = sWorld->FindSession(l_AccountId);
        if (!l_Session)
            return;

        if (atLogin & AT_LOGIN_LOCKED_FOR_TRANSFER)
        {
            WorldPacket data(SMSG_CHAR_DELETE, 1);
            data << uint8(CHAR_DELETE_FAILED_LOCKED_FOR_TRANSFER);
            l_Session->SendPacket(&data);
            return;
        }

        std::string IP_str = l_Session->GetRemoteAddress();
        sLog->outInfo(LOG_FILTER_CHARACTER, "Account: %d (IP: %s) Delete Character:[%s] (GUID: %u)", l_Session->GetAccountId(), IP_str.c_str(), name.c_str(), GUID_LOPART(charGuid));
        sScriptMgr->OnPlayerDelete(charGuid);
        sWorld->DeleteCharacterInfo(GUID_LOPART(charGuid));

        sGuildFinderMgr->RemoveAllMembershipRequestsFromPlayer(charGuid);
        Player::DeleteFromDB(charGuid, l_AccountId);

        WorldPacket data(SMSG_CHAR_DELETE, 1);
        data << uint8(CHAR_DELETE_SUCCESS);
        l_Session->SendPacket(&data);
    });
}

void WorldSession::HandlePlayerLoginOpcode(WorldPacket& p_RecvData)
{
    if (PlayerLoading() || GetPlayer() != NULL)
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Player tries to login again, AccountId = %d", GetAccountId());
        return;
    }

    //////////////////////////////////////////////////////////////////////////

    uint64 l_PlayerGuid = 0;
    float l_FarClip = 0.0f;

    p_RecvData.readPackGUID(l_PlayerGuid);                                  ///< uint64
    p_RecvData >> l_FarClip;                                                ///< float

    //////////////////////////////////////////////////////////////////////////

    if (!CharCanLogin(GUID_LOPART(l_PlayerGuid)))
    {
        sLog->outError(LOG_FILTER_NETWORKIO, "Account (%u) can't login with that character (%u).", GetAccountId(), GUID_LOPART(l_PlayerGuid));
        KickPlayer();
        return;
    }

    m_playerLoading = l_PlayerGuid;

    SendConnectToInstance(WorldPackets::Auth::ConnectToSerial::WorldAttempt1);
}
#endif

void WorldSession::LoginPlayer(uint64 p_Guid)
{
    LoginQueryHolder* l_LoginQueryHolder = new LoginQueryHolder(GetAccountId(), p_Guid, g_RealmID);
    LoginDBQueryHolder* l_LoginDBQueryHolder = new LoginDBQueryHolder(GetAccountId());

    if (!l_LoginQueryHolder->Initialize() || !l_LoginDBQueryHolder->Initialize())
    {
        delete l_LoginQueryHolder;                                      // delete all unprocessed queries
        delete l_LoginDBQueryHolder;                                    // delete all unprocessed queries
        m_playerLoading = 0;
        return;
    }

#ifndef CROSS
    if (m_CanResumCommms)
    {
        SendPacket(WorldPackets::Auth::ResumeComms().Write());
        m_CanResumCommms = false;
    }
#endif

    m_CharacterLoginCallback = CharacterDatabase.DelayQueryHolder((SQLQueryHolder*)l_LoginQueryHolder);
    m_CharacterLoginDBCallback = LoginDatabase.DelayQueryHolder((SQLQueryHolder*)l_LoginDBQueryHolder);
}

void WorldSession::HandleLoadScreenOpcode(WorldPacket& recvPacket)
{
    sLog->outInfo(LOG_FILTER_GENERAL, "WORLD: Recvd CMSG_LOAD_SCREEN");
    uint32 mapID;

    recvPacket >> mapID;
    recvPacket.ReadBit();
}

#ifndef CROSS
void WorldSession::HandlePlayerLogin(LoginQueryHolder* l_CharacterHolder, LoginDBQueryHolder* l_LoginHolder)
{
    uint64 playerGuid = l_CharacterHolder->GetGuid();

    Player* pCurrChar = new Player(this);

    sLog->outInfo(LOG_FILTER_POINTERS, "Pointer 0x%" PRIxPTR " -> Player Guid: %u", std::uintptr_t(pCurrChar), GUID_LOPART(playerGuid));

    // "GetAccountId() == db stored account id" checked in LoadFromDB (prevent login not own character using cheating tools)
    if (!pCurrChar->LoadFromDB(GUID_LOPART(playerGuid), l_CharacterHolder, l_LoginHolder))
    {
        SetPlayer(NULL);
        KickPlayer();                                       // disconnect client, player no set to session and it will not deleted or saved at kick
        delete pCurrChar;                                   // delete it manually
        delete l_CharacterHolder;                           // delete all unprocessed queries
        delete l_LoginHolder;                               // delete all unprocessed queries
        m_playerLoading = 0;
    }
}
#endif

void FillItemList(std::list<ItemTemplate const*> &p_ItemList, Player* p_Player)
{
    std::vector<std::vector<uint32>>    l_Items;
    std::set<uint32>                    l_ItemsToAdd;


    uint32 l_SpecID = p_Player->GetActiveSpecializationID();
    uint32 l_ItemCount = 0;

    l_Items.resize(InventoryType::MAX_INVTYPE);

    /// Fill possible equipment for each slots
    for (uint8 l_Iter = 0; l_Iter < InventoryType::MAX_INVTYPE; ++l_Iter)
    {
        l_Items[l_Iter].clear();

        for (auto l_Itr : *GetChallengeMaps())
        {
            std::vector<uint32> l_PossibleLoots = GetItemLootPerMap(l_Itr);

            for (uint32 l_ItemId : l_PossibleLoots)
            {
                ItemTemplate const* l_ItemTpl = sObjectMgr->GetItemTemplate(l_ItemId);
                if (!l_ItemTpl)
                    continue;

                if (!l_ItemTpl->IsUsableBySpecialization(p_Player->GetLootSpecId() ? p_Player->GetLootSpecId() : p_Player->GetActiveSpecializationID(), p_Player->getLevel()))
                    continue;

                if (l_ItemTpl->InventoryType != l_Iter)
                    continue;

                if (p_Player->CanUseItem(l_ItemTpl) != InventoryResult::EQUIP_ERR_OK)
                    continue;

                l_Items[l_Iter].push_back(l_ItemTpl->ItemId);

                break;
            }
        }

        std::random_shuffle(l_Items[l_Iter].begin(), l_Items[l_Iter].end());
    }

    for (uint8 l_Iter = 0; l_Iter < InventoryType::MAX_INVTYPE; ++l_Iter)
    {
        uint8 l_Count = 1;

        switch (l_Iter)
        {
        case InventoryType::INVTYPE_FINGER:
        case InventoryType::INVTYPE_TRINKET:
        case InventoryType::INVTYPE_WEAPON:
            l_Count++;
            break;
        default:
            break;
        }

        for (uint32 l_Item : l_Items[l_Iter])
        {
            if (!l_Count)
                break;

            if (l_ItemsToAdd.find(l_Item) != l_ItemsToAdd.end())
                continue;

            l_ItemsToAdd.insert(l_Item);
            --l_Count;
            ++l_ItemCount;
        }
    }

    for (auto l_Itr : l_ItemsToAdd)
        p_ItemList.push_back(sObjectMgr->GetItemTemplate(l_Itr));
}

void WorldSession::FinializePlayerLoading(LoginQueryHolder* p_CharacterHolder, LoginDBQueryHolder* p_LoginHolder)
{
    // for send server info and strings (config)
    ChatHandler l_ChatHandler = ChatHandler(m_Player);

    m_Player->GetMotionMaster()->Initialize();
    m_Player->SendDungeonDifficulty();

    WorldPacket l_Data(SMSG_RESUME_TOKEN, 5);
    l_Data << uint32(0);                                                    ///< Sequence
    l_Data << uint8(0x80);                                                  ///< Reason
    SendPacket(&l_Data);

    l_Data.Initialize(SMSG_LOGIN_VERIFY_WORLD, 24);
    l_Data << m_Player->GetMapId();                                        ///< uint32
    l_Data << m_Player->GetPositionX();                                    ///< float
    l_Data << m_Player->GetPositionY();                                    ///< float
    l_Data << m_Player->GetPositionZ();                                    ///< float
    l_Data << m_Player->GetOrientation();                                  ///< float
    l_Data << uint32(0);                                                   ///< uint32 TransferSpellID
    SendPacket(&l_Data);

    // load player specific part before send times
    LoadAccountData(p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_LOADACCOUNTDATA), PER_CHARACTER_CACHE_MASK);
#ifndef CROSS
    if (!IsBackFromCross())
    {
        SendAccountDataTimes(m_Player->GetGUID());
        SendFeatureSystemStatus();
    }

    /// Send MOTD
    if (!IsBackFromCross())
    {
        MotdText const l_MotdText = sWorld->GetMotd();
        std::string l_MotdStr = "";

        switch (GetSessionDbLocaleIndex())
        {
            case LocaleConstant::LOCALE_frFR:
                l_MotdStr = l_MotdText.TextFR;
                break;
            case LocaleConstant::LOCALE_esMX:
            case LocaleConstant::LOCALE_esES:
                l_MotdStr = l_MotdText.TextES;
                break;
            case LocaleConstant::LOCALE_ruRU:
                l_MotdStr = l_MotdText.TextRU;
                break;
            default:
                l_MotdStr = l_MotdText.Text;
                break;
        }

        std::string::size_type l_Position, l_NextPosition;
        std::vector<std::string> l_Lines;
        uint32 l_LineCount = 0;

        l_Data.Initialize(SMSG_MOTD, 100);                     // new in 2.0.1

        l_Position = 0;
        while ((l_NextPosition = l_MotdStr.find('@', l_Position)) != std::string::npos)
        {
            if (l_NextPosition != l_Position)
            {
                l_Lines.push_back(l_MotdStr.substr(l_Position, l_NextPosition - l_Position));
                ++l_LineCount;
            }

            l_Position = l_NextPosition + 1;
        }

        if (l_Position < l_MotdStr.length())
        {
            l_Lines.push_back(l_MotdStr.substr(l_Position));
            ++l_LineCount;
        }

        l_Data.WriteBits(l_Lines.size(), 4);
        l_Data.FlushBits();

        for (size_t l_I = 0; l_I < l_Lines.size(); l_I++)
        {
            l_Data.WriteBits(l_Lines[l_I].length(), 7);
            l_Data.FlushBits();
            l_Data.WriteString(l_Lines[l_I]);
        }

        SendPacket(&l_Data);

        /// Send server info
        if (sWorld->getIntConfig(CONFIG_ENABLE_SINFO_LOGIN) == 1)
            l_ChatHandler.PSendSysMessage("%s", GitRevision::GetFullVersion());
    }
#endif
    SendTimeZoneInformations();

    if (sWorld->getBoolConfig(CONFIG_ARENA_SEASON_IN_PROGRESS))
    {
        l_Data.Initialize(SMSG_PVP_SEASON, 8);
        l_Data << uint32(sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));
        l_Data << uint32(sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID) - 1);
        SendPacket(&l_Data);

        m_Player->SendUpdateWorldState(3900, sWorld->getIntConfig(CONFIG_ARENA_SEASON_ID));
    }

    //QueryResult* result = CharacterDatabase.PQuery("SELECT guildid, rank FROM guild_member WHERE guid = '%u'", m_Player->GetGUIDLow());
    if (PreparedQueryResult resultGuild = p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_LOADGUILD))
    {
        Field* fields = resultGuild->Fetch();
        m_Player->SetInGuild(fields[0].GetUInt32());
        m_Player->SetRank(fields[1].GetUInt8());
/// @TODO cross guild
#ifndef CROSS
        if (Guild * guild = sGuildMgr->GetGuildById(m_Player->GetGuildId()))
            m_Player->SetGuildLevel(guild->GetAchievementMgr().GetAchievementPoints());
#endif
    }
/// @TODO cross guild
#ifndef CROSS
    else if (m_Player->GetGuildId())                        // clear guild related fields in case wrong data about non existed membership
    {
        m_Player->SetInGuild(0);
        m_Player->SetRank(0);
        m_Player->SetGuildLevel(0);
    }
#endif

    HotfixData const& hotfix = sObjectMgr->GetHotfixData();

    m_Player->SendInitialPacketsBeforeAddToMap();

    //Show cinematic at the first time that player login
    if (!m_Player->getCinematic())
    {
        m_Player->setCinematic(1);

        if (ChrClassesEntry const* cEntry = sChrClassesStore.LookupEntry(m_Player->getClass()))
        {
            if (m_Player->getClass() == CLASS_DEMON_HUNTER)
                m_Player->SendMovieStart(469);
            else if (m_Player->getRace() == RACE_HIGHMOUNTAIN_TAUREN)
                m_Player->PlayScene(1984, m_Player);
            else if (m_Player->getRace() == RACE_LIGHTFORGED_DRAENEI)
                m_Player->PlayScene(2005, m_Player);
            else if (m_Player->getRace() == RACE_VOID_ELF)
                m_Player->PlayScene(2006, m_Player);
            else if (m_Player->getRace() == RACE_NIGHTBORNE)
                m_Player->PlayScene(2007, m_Player);
            else if (cEntry->CinematicSequenceID)
                m_Player->SendCinematicStart(cEntry->CinematicSequenceID);
            else if (ChrRacesEntry const* rEntry = sChrRacesStore.LookupEntry(m_Player->getRace()))
                m_Player->SendCinematicStart(rEntry->CinematicSequence);

            // send new char string if not empty
            if (!sWorld->GetNewCharString().empty())
                l_ChatHandler.PSendSysMessage("%s", sWorld->GetNewCharString().c_str());
        }
    }

    if (GroupPtr group = m_Player->GetGroup())
    {
        if (group->isLFGGroup())
        {
            LfgDungeonSet Dungeons;
            Dungeons.insert(sLFGMgr->GetDungeon(group->GetGUID()));
            sLFGMgr->SetSelectedDungeons(m_Player->GetGUID(), Dungeons);
            sLFGMgr->SetState(m_Player->GetGUID(), sLFGMgr->GetState(group->GetGUID()));
        }
    }

    if (!m_Player->GetMap()->AddPlayerToMap(m_Player) || !m_Player->CheckInstanceLoginValid())
    {
        AreaTriggerStruct const* at = sObjectMgr->GetGoBackTrigger(m_Player->GetMapId());
        if (at)
            m_Player->TeleportTo(at->target_mapId, at->target_X, at->target_Y, at->target_Z, m_Player->GetOrientation());
        else
            m_Player->TeleportTo(m_Player->m_homebindMapId, m_Player->m_homebindX, m_Player->m_homebindY, m_Player->m_homebindZ, m_Player->GetOrientation());
    }

    sObjectAccessor->AddObject(m_Player);
/// @TODO cross and guild
#ifndef CROSS
    if (m_Player->GetGuildId() != 0 && !IsBackFromCross())
    {
        if (Guild* guild = sGuildMgr->GetGuildById(m_Player->GetGuildId()))
            guild->SendLoginInfo(this);
        else
        {
            // remove wrong guild data
            sLog->outError(LOG_FILTER_GENERAL, "Player %s (GUID: %u) marked as member of not existing guild (id: %u), removing guild membership for player.", m_Player->GetName(), m_Player->GetGUIDLow(), m_Player->GetGuildId());
            m_Player->SetInGuild(0);
        }
    }
#endif
    m_Player->SendInitialPacketsAfterAddToMap();

    CharacterDatabase.PExecute("UPDATE characters SET online = 1 WHERE guid = '%u'", m_Player->GetGUIDLow());
    LoginDatabase.PExecute("UPDATE account SET online = 1 WHERE id = '%u'", GetAccountId());
    m_Player->SetInGameTime(getMSTime());

    // announce group about member online (must be after add to player list to receive announce to self)
    if (GroupPtr group = m_Player->GetGroup())
    {
        //m_Player->groupInfo.group->SendInit(this); // useless
        group->SendUpdate();
        group->ResetMaxEnchantingLevel();
    }

    /// @TODO cross
#ifndef CROSS
    // friend status
    if (!IsBackFromCross())
        sSocialMgr->SendFriendStatus(m_Player, FRIEND_ONLINE, m_Player->GetGUIDLow(), true);
#endif
    // Place character in world (and load zone) before some object loading
    m_Player->LoadCorpse(p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_LOAD_CORPSE_LOCATION));

    bool l_Ghost = true;
    if (InstanceScript* l_Instance = m_Player->GetInstanceScript())
    {
        if (l_Instance->IsEncounterInProgress() && l_Instance->instance->Expansion() >= Expansion::EXPANSION_WARLORDS_OF_DRAENOR)
            l_Ghost = false;
    }

    // setting Ghost+speed if dead
    if (m_Player->m_deathState != ALIVE && l_Ghost)
    {
        // not blizz like, we must correctly save and load player instead...
        if (m_Player->getRace() == RACE_NIGHTELF)
            m_Player->CastSpell(m_Player, 20584, true, 0);// auras SPELL_AURA_INCREASE_SPEED(+speed in wisp form), SPELL_AURA_INCREASE_SWIM_SPEED(+swim speed in wisp form), SPELL_AURA_TRANSFORM (to wisp form)
        m_Player->CastSpell(m_Player, 8326, true, 0);     // auras SPELL_AURA_GHOST, SPELL_AURA_INCREASE_SPEED(why?), SPELL_AURA_INCREASE_SWIM_SPEED(why?)

        m_Player->SetWaterWalking(true);
    }

    m_Player->ContinueTaxiFlight();

    // Set FFA PvP for non GM in non-rest mode
    if (sWorld->IsFFAPvPRealm() && !m_Player->isGameMaster() && !m_Player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_RESTING))
        m_Player->SetByteFlag(UNIT_FIELD_SHAPESHIFT_FORM, UNIT_BYTES_2_OFFSET_PVP_FLAG, UNIT_BYTE2_FLAG_FFA_PVP);

    if (m_Player->HasFlag(PLAYER_FIELD_PLAYER_FLAGS, PLAYER_FLAGS_CONTESTED_PVP))
        m_Player->SetContestedPvP();

    // Apply at_login requests
    if (m_Player->HasAtLoginFlag(AT_LOGIN_RESET_SPELLS))
    {
        m_Player->resetSpells();
        SendNotification(LANG_RESET_SPELLS);
    }

    if (m_Player->HasAtLoginFlag(AT_LOGIN_RESET_TALENTS))
    {
        m_Player->ResetTalents(true);
        m_Player->ResetTalentSpecialization();
        m_Player->SendTalentsInfoData(); // original talents send already in to SendInitialPacketsBeforeAddToMap, resend reset state
        SendNotification(LANG_RESET_TALENTS);
    }

    if (m_Player->HasAtLoginFlag(AT_LOGIN_RESET_SPECS))
    {
        m_Player->ResetTalentSpecialization();
        m_Player->RemoveAtLoginFlag(AT_LOGIN_RESET_SPECS);
    }

    if (m_Player->HasAtLoginFlag(AtLoginFlags::AT_LOGIN_DELETE_INVALID_SPELL))
    {
        m_Player->DeleteInvalidSpells();
        m_Player->UpdateSkillsForLevel();
        m_Player->LearnDefaultSkills();
        m_Player->LearnSpecializationSpells();
        m_Player->RemoveAtLoginFlag(AtLoginFlags::AT_LOGIN_DELETE_INVALID_SPELL);
    }

    if (m_Player->HasAtLoginFlag(AT_LOGIN_FIRST))
    {
        m_Player->RemoveAtLoginFlag(AT_LOGIN_FIRST);

        PlayerInfo const* l_Info = sObjectMgr->GetPlayerInfo(m_Player->getRace(), m_Player->getClass());
        for (uint32 l_SpellId : l_Info->castSpells)
            m_Player->CastSpell(m_Player, l_SpellId, true);

        if (sWorld->getBoolConfig(CONFIG_TOURNAMENT_ENABLE))
        {
            if (m_Player->getRace() != Races::RACE_PANDAREN_NEUTRAL)
            {
                if (m_Player->GetTeamId() == TeamId::TEAM_HORDE)
                {
                    /// Add mounts
                    m_Player->AddItem(49046,  1);
                    m_Player->AddItem(107951, 1);
                }

                if (m_Player->GetTeamId() == TeamId::TEAM_ALLIANCE)
                {
                    /// Add mounts
                    m_Player->AddItem(47180, 1);
                    m_Player->AddItem(98259, 1);
                }

                if (m_Player->GetTeamId() == TeamId::TEAM_NEUTRAL)
                    m_Player->ShowNeutralPlayerFactionSelectUI();
            }
        }
    }

    if (m_Player->HasAtLoginFlag(AT_LOGIN_BORIS_ITEMS))
    {
        std::list<ItemTemplate const*> l_Items;
        FillItemList(l_Items, m_Player);

        /// Bags
        for (int i = 0; i < 4; i++)
            l_Items.push_back(sObjectMgr->GetItemTemplate(933));

        /// Mounts
        uint32 l_MountItemId = 0;
        switch (m_Player->getRace())
        {
            case Races::RACE_HUMAN:            l_MountItemId = 5656;  break;
            case Races::RACE_GNOME:            l_MountItemId = 13321; break;
            case Races::RACE_DWARF:            l_MountItemId = 5872;  break;
            case Races::RACE_NIGHTELF:         l_MountItemId = 8632;  break;
            case Races::RACE_ORC:              l_MountItemId = 5668;  break;
            case Races::RACE_TROLL:            l_MountItemId = 8588;  break;
            case Races::RACE_UNDEAD_PLAYER:    l_MountItemId = 13333; break;
            case Races::RACE_BLOODELF:         l_MountItemId = 29220; break;
            case Races::RACE_DRAENEI:          l_MountItemId = 29743; break;
            case Races::RACE_PANDAREN_ALLIANCE:
            case Races::RACE_PANDAREN_HORDE:
            case Races::RACE_PANDAREN_NEUTRAL: l_MountItemId = 87800; break;
            case Races::RACE_GOBLIN:           l_MountItemId = 62461; break;
            case Races::RACE_WORGEN:           l_MountItemId = 73838; break;
            case Races::RACE_TAUREN:           l_MountItemId = 15290; break;
        }

        if (l_MountItemId)
            l_Items.push_back(sObjectMgr->GetItemTemplate(l_MountItemId));

        bool l_DoAddItems = true;
        if (m_Player->GetBagsFreeSlots() < l_Items.size())
        {
            int l_FreeBagSlots = 0;
            for (int l_Slot = INVENTORY_SLOT_BAG_START; l_Slot < INVENTORY_SLOT_BAG_END; ++l_Slot)
            {
                if (!m_Player->GetBagByPos(l_Slot))
                    l_FreeBagSlots++;
            }

            if (l_FreeBagSlots < 2)
                l_DoAddItems = false;
        }

        if (l_DoAddItems)
        {
            for (ItemTemplate const* l_ItemTemplate : l_Items)
                m_Player->StoreNewItemInBestSlots(l_ItemTemplate->ItemId, 1, ItemContext::WorldQuest10);

            m_Player->RemoveAtLoginFlag(AT_LOGIN_BORIS_ITEMS);
        }
    }

    // show time before shutdown if shutdown planned.
    if (sWorld->IsShuttingDown())
        sWorld->ShutdownMsg(true, m_Player);

    if (sWorld->getBoolConfig(CONFIG_ALL_TAXI_PATHS))
        m_Player->SetTaxiCheater(true);

    if (m_Player->isGameMaster())
        SendNotification(LANG_GM_ON);
    /// @TODO cross
#ifndef CROSS
    if (!IsBackFromCross())
    {
        m_Player->SendCUFProfiles();
        m_Player->SendToyBox();
    }
#endif
    // Hackfix Remove Talent spell - Remove Glyph spell
    m_Player->learnSpell(111621, false); // Reset Glyph
    m_Player->learnSpell(113873, false); // Reset Talent

    std::string IP_str = GetRemoteAddress();
    sLog->outInfo(LOG_FILTER_CHARACTER, "Account: %d (IP: %s) Login Character:[%s] (GUID: %u) Level: %d",
        GetAccountId(), IP_str.c_str(), m_Player->GetName(), m_Player->GetGUIDLow(), m_Player->getLevel());

    if (!m_Player->IsStandState() && !m_Player->HasUnitState(UNIT_STATE_STUNNED))
        m_Player->SetStandState(UNIT_STAND_STATE_STAND);

    m_playerLoading = 0;

    // fix exploit with Aura Bind Sight
    m_Player->StopCastingBindSight();
    m_Player->StopCastingCharm();
    m_Player->RemoveAurasByType(SPELL_AURA_BIND_SIGHT);

    // Fix chat with transfert / rename
    sWorld->AddCharacterInfo(m_Player->GetGUIDLow(), m_Player->GetName(), GetAccountId(), m_Player->getGender(), m_Player->getRace(), m_Player->getClass(), m_Player->getLevel(), m_Player->GetAtLoginFlags());

    /// Remove title due to exploit with first achievement
    m_Player->SetTitle(sCharTitlesStore.LookupEntry(139), true);
    m_Player->SetTitle(sCharTitlesStore.LookupEntry(120), true);
    m_Player->SetTitle(sCharTitlesStore.LookupEntry(122), true);
    m_Player->SetTitle(sCharTitlesStore.LookupEntry(159), true);

    m_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_HONOR_LEVEL_REACHED, m_Player->GetHonorLevel());
    m_Player->UpdateCriteria(CriteriaTypes::CRITERIA_TYPE_PRESTIGE_REACHED, m_Player->GetPrestige());

#ifndef CROSS
    //if (!IsBackFromCross())
        sScriptMgr->OnPlayerLogin(m_Player);
#endif

    CharTitlesEntry const* l_LegionTitle = sCharTitlesStore.LookupEntry(22);
    if (!m_Player->HasTitle(l_LegionTitle) && m_Player->GetGUIDLow() <= 72197)
        m_Player->SetTitle(l_LegionTitle);

    PreparedQueryResult l_ItemResult       = p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_BOUTIQUE_ITEM);
    PreparedQueryResult l_GoldResult       = p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_BOUTIQUE_GOLD);
    PreparedQueryResult l_TitleResult      = p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_BOUTIQUE_TITLE);
    PreparedQueryResult l_LevelResult      = p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_BOUTIQUE_LEVEL);
    PreparedQueryResult l_ProfessionResult = p_CharacterHolder->GetPreparedResult(PLAYER_LOGIN_QUERY_STORE_PROFESSION);

    m_Player->HandleStoreItemCallback(l_ItemResult);
    m_Player->HandleStoreGoldCallback(l_GoldResult);
    m_Player->HandleStoreTitleCallback(l_TitleResult);
    m_Player->HandleStoreLevelCallback(l_LevelResult);
    m_Player->HandleStoreProfessionCallback(l_ProfessionResult);

    if (l_ItemResult || l_GoldResult || l_TitleResult || l_LevelResult || l_ProfessionResult)
        m_Player->SaveToDB();

#ifndef CROSS
    if (IsBackFromCross())
    {
        RestoreChannels();
        m_Player->ScheduleDelayedOperation(DELAYED_BG_TAXI_RESTORE);
        m_Player->ProcessDelayedOperations();
    }

    SetBackFromCross(false);
#endif

#ifdef CROSS
    GetPlayer()->SetPlayOnCross(true);
    p_CharacterHolder->Callback();
#endif

    sPOIMgr->InitializeForPlayer(m_Player);
    sContributionMgr->InitializeForPlayer(m_Player);

    m_Player->m_TaskInProgress = false;

    delete p_CharacterHolder;
    delete p_LoginHolder;
}

void WorldSession::HandleSetFactionAtWar(WorldPacket& p_Packet)
{
    uint8 l_FactionIndex;
    p_Packet >> l_FactionIndex;

    GetPlayer()->GetReputationMgr().SetAtWar(l_FactionIndex, true);
}

void WorldSession::HandleUnSetFactionAtWar(WorldPacket& p_Packet)
{
    uint8 l_FactionIndex;
    p_Packet >> l_FactionIndex;

    GetPlayer()->GetReputationMgr().SetAtWar(l_FactionIndex, false);
}

enum TutorialAction : uint8
{
    TUTORIAL_ACTION_RESET   = 1,
    TUTORIAL_ACTION_CLEAR   = 2,
    TUTORIAL_ACTION_FLAG    = 3,
};

/*
enum Tutorials
{
    TUTORIAL_TALENT                   = 0,
    TUTORIAL_SPEC                     = 1,
    TUTORIAL_GLYPH                    = 2,
    TUTORIAL_SPELLBOOK                = 3,
    TUTORIAL_PROFESSIONS              = 4,
    TUTORIAL_CORE_ABILITITES          = 5,
    TUTORIAL_PET_JOURNAL              = 6,
    TUTORIAL_WHAT_HAS_CHANGED         = 7,
    TUTORIAL_GARRISON_BUILDING        = 8,
    TUTORIAL_GARRISON_MISSION_LIST    = 9,
    TUTORIAL_GARRISON_MISSION_PAGE    = 10,
    TUTORIAL_GARRISON_LANDING         = 11,
    TUTORIAL_GARRISON_ZONE_ABILITY    = 12,
    TUTORIAL_WORLD_MAP_FRAME          = 13,
    TUTORIAL_CLEAN_UP_BAGS            = 14,
    TUTORIAL_BAG_SETTINGS             = 15,
    TUTORIAL_REAGENT_BANK_UNLOCK      = 16,
    TUTORIAL_TOYBOX_FAVORITE          = 17,
    TUTORIAL_TOYBOX_MOUSEWHEEL_PAGING = 18,
    TUTORIAL_LFG_LIST                 = 19
};
*/

void WorldSession::HandleTutorial(WorldPacket& p_RecvPacket)
{
    uint32 l_Action;

    l_Action = p_RecvPacket.ReadBits(2);

    switch (l_Action)
    {
        case TUTORIAL_ACTION_FLAG:
            {
                uint32 l_TutorialBit;
                p_RecvPacket >> l_TutorialBit;

                uint8 l_Index = uint8(l_TutorialBit / 32);

                if (l_Index >= MAX_ACCOUNT_TUTORIAL_VALUES)
                    return;

                uint32 l_Value = (l_TutorialBit % 32);

                uint32 l_Flag = GetTutorialInt(l_Index);
                l_Flag |= (1 << l_Value);

                SetTutorialInt(l_Index, l_Flag);
            }
            break;

        case TUTORIAL_ACTION_CLEAR:
            for (uint8 l_I = 0; l_I < MAX_ACCOUNT_TUTORIAL_VALUES; ++l_I)
                SetTutorialInt(l_I, 0xFFFFFFFF);
            break;

        case TUTORIAL_ACTION_RESET:
            for (uint8 l_I = 0; l_I < MAX_ACCOUNT_TUTORIAL_VALUES; ++l_I)
                SetTutorialInt(l_I, 0x00000000);
            break;
    }
}

void WorldSession::HandleSetWatchedFactionOpcode(WorldPacket& recvData)
{
    uint32 fact;
    recvData >> fact;
    GetPlayer()->SetUInt32Value(PLAYER_FIELD_WATCHED_FACTION_INDEX, fact);
}

void WorldSession::HandleSetFactionInactiveOpcode(WorldPacket& recvData)
{
    uint32 replistid;
    bool inactive;

    recvData >> replistid;
    inactive = recvData.ReadBit();

    m_Player->GetReputationMgr().SetInactive(replistid, inactive);
}

#ifndef CROSS
void WorldSession::HandleCharRenameOpcode(WorldPacket& p_RecvData)
{
    uint64 l_Guid     = 0;
    uint32 nameLen  = 0;
    std::string newName;

    p_RecvData.readPackGUID(l_Guid);
    nameLen = p_RecvData.ReadBits(6);
    newName = p_RecvData.ReadString(nameLen);

    // prevent character rename to invalid name
    if (!normalizePlayerName(newName))
    {
        WorldPacket data(SMSG_CHAR_RENAME);
        BuildCharacterRename(&data, 0, CHAR_NAME_NO_NAME, "");
        SendPacket(&data);
        return;
    }

    uint8 res = ObjectMgr::CheckPlayerName(newName, true);
    if (res != CHAR_NAME_SUCCESS)
    {
        WorldPacket data(SMSG_CHAR_RENAME);
        BuildCharacterRename(&data, l_Guid, res, newName);
        SendPacket(&data);
        return;
    }

    // check name limitations
    if (AccountMgr::IsPlayerAccount(GetSecurity()) && sObjectMgr->IsReservedName(newName))
    {
        WorldPacket data(SMSG_CHAR_RENAME);
        BuildCharacterRename(&data, 0, CHAR_NAME_RESERVED, "");
        SendPacket(&data);
        return;
    }

    // Ensure that the character belongs to the current account, that rename at login is enabled
    // and that there is no character with the desired new name
    _charRenameCallback.SetParam(newName);

    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_SEL_FREE_NAME);

    stmt->setUInt32(0, GUID_LOPART(l_Guid));
    stmt->setUInt32(1, GetAccountId());
    stmt->setUInt16(2, AT_LOGIN_RENAME);
    stmt->setUInt16(3, AT_LOGIN_RENAME);
    stmt->setString(4, newName);
    stmt->setString(5, newName);

    _charRenameCallback.SetFutureResult(CharacterDatabase.AsyncQuery(stmt));
}

void WorldSession::BuildCharacterRename(WorldPacket* p_Packet, ObjectGuid p_Guid, uint8 p_Result, std::string p_Name)
{
    *p_Packet << uint8(p_Result);
    p_Packet->WriteBit(p_Guid != 0);
    p_Packet->WriteBits(p_Name.size(), 6);
    p_Packet->FlushBits();

    if (p_Guid != 0)
        p_Packet->appendPackGUID(p_Guid);

    p_Packet->WriteString(p_Name);
}

void WorldSession::HandleChangePlayerNameOpcodeCallBack(PreparedQueryResult result, std::string newName)
{
    if (!result)
    {
        WorldPacket data(SMSG_CHAR_RENAME);
        BuildCharacterRename(&data, 0, CHAR_CREATE_ERROR, newName);
        SendPacket(&data);
        return;
    }

    Field* fields = result->Fetch();

    uint32 guidLow      = fields[0].GetUInt32();
    std::string oldName = fields[1].GetString();

    uint64 guid = MAKE_NEW_GUID(guidLow, 0, HIGHGUID_PLAYER);

    // Update name and at_login flag in the db
    PreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_NAME);

    stmt->setString(0, newName);
    stmt->setUInt16(1, AT_LOGIN_RENAME);
    stmt->setUInt32(2, guidLow);

    CharacterDatabase.Execute(stmt);

    // Removed declined name from db
    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_DECLINED_NAME);

    stmt->setUInt32(0, guidLow);

    CharacterDatabase.Execute(stmt);

    // Logging
    stmt = LogDatabase.GetPreparedStatement(LOG_INSERT_NAME_LOG);

    stmt->setUInt32(0, guidLow);
    stmt->setString(1, oldName);
    stmt->setString(2, newName);

    LogDatabase.Execute(stmt);

    sLog->outInfo(LOG_FILTER_CHARACTER, "Account: %d (IP: %s) Character:[%s] (guid:%u) Changed name to: %s", GetAccountId(), GetRemoteAddress().c_str(), oldName.c_str(), guidLow, newName.c_str());

    WorldPacket data(SMSG_CHAR_RENAME);
    BuildCharacterRename(&data, guid, RESPONSE_SUCCESS, newName);
    SendPacket(&data);

    sWorld->UpdateCharacterInfo(guidLow, newName);
    sWorld->RemoveCharacterInfoLoginFlag(guidLow, AtLoginFlags::AT_LOGIN_RENAME);
}
#endif

void WorldSession::HandleSetPlayerDeclinedNames(WorldPacket& p_RecvData)
{
#ifndef CROSS
    uint64       l_Player;
    uint8        l_DeclinedNameSize[5];
    DeclinedName l_DeclinedName;

    p_RecvData.readPackGUID(l_Player);

    for (int l_I = 0; l_I < MAX_DECLINED_NAME_CASES; l_I++)
        l_DeclinedNameSize[l_I] = p_RecvData.ReadBits(7);

    for (int l_I = 0; l_I < MAX_DECLINED_NAME_CASES; l_I++)
    {
        l_DeclinedName.name[l_I] = p_RecvData.ReadString(l_DeclinedNameSize[l_I]);
        if (!normalizePlayerName(l_DeclinedName.name[l_I]))
        {
            SendPlayerDeclinedNamesResult(l_Player, 1);
            return;
        }
    }

    // not accept declined names for unsupported languages
    std::string l_Name;
    if (!sObjectMgr->GetPlayerNameByGUID(l_Player, l_Name))
    {
        SendPlayerDeclinedNamesResult(l_Player, 1);
        return;
    }

    std::wstring l_WName;
    if (!Utf8toWStr(l_Name, l_WName))
    {
        SendPlayerDeclinedNamesResult(l_Player, 1);
        return;
    }

    if (!isCyrillicCharacter(l_WName[0]))                      // name already stored as only single alphabet using
    {
        SendPlayerDeclinedNamesResult(l_Player, 1);
        return;
    }

    if (!ObjectMgr::CheckDeclinedNames(l_WName, l_DeclinedName))
    {
        SendPlayerDeclinedNamesResult(l_Player, 1);
        return;
    }

    for (int l_I = 0; l_I < MAX_DECLINED_NAME_CASES; ++l_I)
        CharacterDatabase.EscapeString(l_DeclinedName.name[l_I]);

    SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();

    PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_DECLINED_NAME);
    l_Statement->setUInt32(0, GUID_LOPART(l_Player));
    l_Transaction->Append(l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_DECLINED_NAME);
    l_Statement->setUInt32(0, GUID_LOPART(l_Player));

    for (uint8 l_I = 0; l_I < MAX_DECLINED_NAME_CASES; l_I++)
        l_Statement->setString(l_I + 1, l_DeclinedName.name[l_I]);

    l_Transaction->Append(l_Statement);

    CharacterDatabase.CommitTransaction(l_Transaction);

    SendPlayerDeclinedNamesResult(l_Player, 0);
#endif
}

void WorldSession::SendPlayerDeclinedNamesResult(uint64 p_Player, uint32 p_Result)
{
    WorldPacket data(SMSG_SET_PLAYER_DECLINED_NAMES_RESULT, 4 + 8);
    data << uint32(p_Result);
    data.appendPackGUID(p_Player);
    SendPacket(&data);
}

void WorldSession::HandleAlterAppearance(WorldPacket& p_RecvData)
{
    uint32 l_NewHair, l_NewColor, l_NewFacialHair, l_NewSkinColor, l_NewFace;
    p_RecvData >> l_NewHair >> l_NewColor >> l_NewFacialHair >> l_NewSkinColor >> l_NewFace;

    std::array<uint32, PLAYER_CUSTOM_DISPLAY_SIZE> l_NewCustomDisplay;
    for (uint8 l_I = 0; l_I < PLAYER_CUSTOM_DISPLAY_SIZE; ++l_I)
        l_NewCustomDisplay[l_I] = p_RecvData.read<uint32>();

    BarberShopStyleEntry const* l_BsHair = sBarberShopStyleStore.LookupEntry(l_NewHair);
    if (!l_BsHair || l_BsHair->Type != 0 || l_BsHair->Race != m_Player->getRace() || l_BsHair->Sex != m_Player->getGender())
        return;

    BarberShopStyleEntry const* l_BsFacialHair = sBarberShopStyleStore.LookupEntry(l_NewFacialHair);
    if (!l_BsFacialHair || l_BsFacialHair->Type != 2 || l_BsFacialHair->Race != m_Player->getRace() || l_BsFacialHair->Sex != m_Player->getGender())
        return;

    BarberShopStyleEntry const* l_BsSkinColor = sBarberShopStyleStore.LookupEntry(l_NewSkinColor);
    if (l_BsSkinColor && (l_BsSkinColor->Type != 3 || l_BsSkinColor->Race != m_Player->getRace() || l_BsSkinColor->Sex != m_Player->getGender()))
        return;

    BarberShopStyleEntry const* l_BsFace = sBarberShopStyleStore.LookupEntry(l_NewFace);
    if (l_BsFace && (l_BsFace->Type != 4 || l_BsFace->Race != m_Player->getRace() || l_BsFace->Sex != m_Player->getGender()))
        return;

    std::array<BarberShopStyleEntry const*, PLAYER_CUSTOM_DISPLAY_SIZE> l_CustomDisplayEntries;
    std::array<uint8, PLAYER_CUSTOM_DISPLAY_SIZE> l_CustomDisplay;

    for (uint8 l_I = 0; l_I < PLAYER_CUSTOM_DISPLAY_SIZE; ++l_I)
    {
        BarberShopStyleEntry const* l_BsCustomDisplay = sBarberShopStyleStore.LookupEntry(l_NewCustomDisplay[l_I]);
        if (l_BsCustomDisplay == nullptr)
        {
            l_CustomDisplayEntries[l_I] = nullptr;
            continue;
        }

        if (l_BsCustomDisplay->Type != 5 + l_I || l_BsCustomDisplay->Race != m_Player->getRace() || l_BsCustomDisplay->Sex != m_Player->getGender())
        {
            l_CustomDisplayEntries[l_I] = nullptr;
            continue;
        }

        l_CustomDisplay[l_I] = l_BsCustomDisplay->Data;
        l_CustomDisplayEntries[l_I] = l_BsCustomDisplay;
    }

    /// @TODO: Fix it
    /*if (!Player::ValidateAppearance(m_Player->getRace(), m_Player->getClass(), m_Player->getGender(),
        l_BsHair->Data,
        l_NewColor,
        l_BsFace ? l_BsFace->Data : m_Player->GetFace(),
        l_BsFacialHair->Data,
        l_BsSkinColor ? l_BsSkinColor->Data : m_Player->GetSkin(),
        l_CustomDisplay))
        return;*/

    GameObject* l_Chair = m_Player->FindNearestGameObjectOfType(GameobjectTypes::GAMEOBJECT_TYPE_BARBER_CHAIR, 5.0f);
    if (!l_Chair || (m_Player->getStandState() != UNIT_STAND_STATE_SIT_LOW_CHAIR + l_Chair->GetGOInfo()->barberChair.chairheight))
    {
        SendBarberShopResult(BarberShopResult::BARBER_SHOP_RESULT_NOT_ON_CHAIR);
        return;
    }

    uint32 l_Cost = m_Player->GetBarberShopCost(l_BsHair->Data, l_NewColor, l_BsFacialHair->Data, l_BsSkinColor, l_BsFace, l_CustomDisplayEntries);

    if (!m_Player->HasEnoughMoney((uint64)l_Cost))
    {
        SendBarberShopResult(BarberShopResult::BARBER_SHOP_RESULT_NO_MONEY);
        return;
    }
    else
        SendBarberShopResult(BarberShopResult::BARBER_SHOP_RESULT_SUCCESS);

    m_Player->ModifyMoney(-int64(l_Cost), "HandleAlterAppearance");                     // it isn't free
    m_Player->UpdateCriteria(CRITERIA_TYPE_GOLD_SPENT_AT_BARBER, l_Cost);

    m_Player->SetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_HAIR_STYLE_ID, uint8(l_BsHair->Data));
    m_Player->SetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_HAIR_COLOR_ID, uint8(l_NewColor));
    m_Player->SetByteValue(PLAYER_FIELD_CUSTOM_DISPLAY_OPTION, PLAYER_CUSTOM_OPTOTIONS_FACIAL_STYLE, uint8(l_BsFacialHair->Data));

    if (l_BsSkinColor)
        m_Player->SetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_SKIN_ID, uint8(l_BsSkinColor->Data));

    if (l_BsFace)
        m_Player->SetByteValue(PLAYER_FIELD_HAIR_COLOR_ID, PLAYER_FIELD_HAIR_COLOR_ID_OFFSET_FACE_ID, l_BsFace->Data);

    for (uint8 l_I = 0; l_I < PLAYER_CUSTOM_DISPLAY_SIZE; ++l_I)
        m_Player->SetByteValue(PLAYER_FIELD_CUSTOM_DISPLAY_OPTION, PLAYER_BYTES_2_OFFSET_CUSTOM_DISPLAY_OPTION + l_I, l_CustomDisplay[l_I]);

    m_Player->SavePlayerBytes(m_Player->GetUInt32Value(PLAYER_FIELD_HAIR_COLOR_ID));

    m_Player->UpdateCriteria(CRITERIA_TYPE_VISIT_BARBER_SHOP, 1);

    m_Player->SetStandState(0);                              // stand up
}

void WorldSession::SendBarberShopResult(BarberShopResult p_Result)
{
    WorldPacket l_Data(Opcodes::SMSG_BARBER_SHOP_RESULT, 4);
    l_Data << uint32(p_Result);
    SendPacket(&l_Data);
}

void WorldSession::SendCharCustomize(ResponseCodes p_Error, uint64 p_Guid)
{
    WorldPacket l_Data(Opcodes::SMSG_CHAR_CUSTOMIZE_FAILED, 1);
    l_Data << uint8(p_Error);
    l_Data.appendPackGUID(p_Guid);
    SendPacket(&l_Data);
}

void WorldSession::SendCharFactionChange(ResponseCodes p_Error, uint64 p_Guid)
{
    WorldPacket l_Data(Opcodes::SMSG_CHAR_FACTION_CHANGE_RESULT, 1);
    l_Data << uint8(p_Error);
    l_Data.appendPackGUID(p_Guid);
    SendPacket(&l_Data);
}

#ifndef CROSS
void WorldSession::HandleCharCustomize(WorldPacket& p_RecvData)
{
    uint64 l_PlayerGuid = 0;
    uint8 l_CharacterGender     = 0;
    uint8 l_CharacterSkin       = 0;
    uint8 l_CharacterFace       = 0;
    uint8 l_CharacterHairStyle  = 0;
    uint8 l_CharacterHairColor  = 0;
    uint8 l_CharacterFacialHair = 0;

    std::string l_NewName;
    uint32 l_NameLen;

    p_RecvData.readPackGUID(l_PlayerGuid);

    p_RecvData >> l_CharacterGender;                                        ///< uint8
    p_RecvData >> l_CharacterSkin;                                          ///< uint8
    p_RecvData >> l_CharacterHairColor;                                     ///< uint8
    p_RecvData >> l_CharacterHairStyle;                                     ///< uint8
    p_RecvData >> l_CharacterFacialHair;                                    ///< uint8
    p_RecvData >> l_CharacterFace;                                          ///< uint8

    std::array<uint8, PLAYER_CUSTOM_DISPLAY_SIZE> l_CustomDisplay;
    for (uint8 l_I = 0; l_I < PLAYER_CUSTOM_DISPLAY_SIZE; ++l_I)
        p_RecvData >> l_CustomDisplay[l_I];

    l_NameLen = p_RecvData.ReadBits(6);
    l_NewName = p_RecvData.ReadString(l_NameLen);

    PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_CUSTOMIZE_DATA);
    l_Statement->setUInt32(0, GUID_LOPART(l_PlayerGuid));
    PreparedQueryResult l_Result = CharacterDatabase.Query(l_Statement);

    if (!l_Result)
    {
        SendCharCustomize(ResponseCodes::CHAR_CREATE_ERROR, l_PlayerGuid);
        return;
    }

    Field* l_Fields = l_Result->Fetch();
    uint32 l_AtLoginFlag = l_Fields[0].GetUInt16();
    uint8 l_Race = l_Fields[1].GetUInt8();
    uint8 l_Class = l_Fields[2].GetUInt8();

    if (!(l_AtLoginFlag & AT_LOGIN_CUSTOMIZE))
    {
        SendCharCustomize(ResponseCodes::CHAR_CREATE_ERROR, l_PlayerGuid);
        return;
    }

    /// @TODO: Fix it
    /*if (!Player::ValidateAppearance(l_Race, l_Class, l_CharacterGender, l_CharacterHairStyle, l_CharacterHairColor, l_CharacterFace, l_CharacterFacialHair, l_CharacterSkin, l_CustomDisplay))
    {
        SendCharCustomize(ResponseCodes::CHAR_CREATE_ERROR, l_PlayerGuid);
        return;
    }*/

    // prevent character rename to invalid name
    if (!normalizePlayerName(l_NewName))
    {
        SendCharCustomize(ResponseCodes::CHAR_NAME_NO_NAME, l_PlayerGuid);
        return;
    }

    uint8 l_Res = ObjectMgr::CheckPlayerName(l_NewName, true);
    if (l_Res != CHAR_NAME_SUCCESS)
    {
        SendCharCustomize(ResponseCodes(l_Res), l_PlayerGuid);
        return;
    }

    // check name limitations
    if (AccountMgr::IsPlayerAccount(GetSecurity()) && sObjectMgr->IsReservedName(l_NewName))
    {
        SendCharCustomize(ResponseCodes::CHAR_NAME_RESERVED, l_PlayerGuid);
        return;
    }

    // character with this name already exist
    if (uint64 l_NewGuid = sWorld->GetCharacterGuidByName(l_NewName))
    {
        if (l_NewGuid != l_PlayerGuid)
        {
            SendCharCustomize(ResponseCodes::CHAR_CREATE_NAME_IN_USE, l_PlayerGuid);
            return;
        }
    }

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHARACTER_NAME);
    l_Statement->setUInt32(0, GUID_LOPART(l_PlayerGuid));
    l_Result = CharacterDatabase.Query(l_Statement);

    if (l_Result)
    {
        std::string oldname = l_Result->Fetch()[0].GetString();
        sLog->outInfo(LOG_FILTER_CHARACTER, "Account: %d (IP: %s), Character[%s] (guid:%u) Customized to: %s", GetAccountId(), GetRemoteAddress().c_str(), oldname.c_str(), GUID_LOPART(l_PlayerGuid), l_NewName.c_str());
    }

    Player::Customize(l_PlayerGuid, l_CharacterGender, l_CharacterSkin, l_CharacterFace, l_CharacterHairStyle, l_CharacterHairColor, l_CharacterFacialHair, l_CustomDisplay);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_NAME_AT_LOGIN);

    l_Statement->setString(0, l_NewName);
    l_Statement->setUInt16(1, uint16(AT_LOGIN_CUSTOMIZE));
    l_Statement->setUInt32(2, GUID_LOPART(l_PlayerGuid));

    CharacterDatabase.Execute(l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_DECLINED_NAME);

    l_Statement->setUInt32(0, GUID_LOPART(l_PlayerGuid));

    CharacterDatabase.Execute(l_Statement);

    sWorld->UpdateCharacterInfo(GUID_LOPART(l_PlayerGuid), l_NewName, l_CharacterGender);

    WorldPacket l_Data(SMSG_CHAR_CUSTOMIZE);
    l_Data.appendPackGUID(l_PlayerGuid);
    l_Data << uint8(l_CharacterGender);
    l_Data << uint8(l_CharacterSkin);
    l_Data << uint8(l_CharacterHairColor);
    l_Data << uint8(l_CharacterHairStyle);
    l_Data << uint8(l_CharacterFacialHair);
    l_Data << uint8(l_CharacterFace);

    for (uint8 l_I = 0; l_I < PLAYER_CUSTOM_DISPLAY_SIZE; ++l_I)
        l_Data << uint8(l_CustomDisplay[l_I]);

    l_Data.WriteBits(l_NewName.size(), 6);
    l_Data.FlushBits();
    l_Data.WriteString(l_NewName);

    SendPacket(&l_Data);
}
#endif

void WorldSession::HandleEquipmentSetSave(WorldPacket& p_RecvData)
{
    uint64 l_SetGuid = 0;
    uint32 l_SetID;
    EquipmentSet l_EquipmentSet;
    uint8 l_IconNameLen, l_SetNameLen;

    uint32 l_Type = p_RecvData.read<uint32>();
    p_RecvData >> l_SetGuid;
    p_RecvData >> l_SetID;
    uint32 l_IgnoreMask = p_RecvData.read<uint32>();

    std::vector<uint64> l_ItemsGuids(EQUIPMENT_SLOT_END, 0);
    std::vector<int32> l_ItemAppearances(EQUIPMENT_SLOT_END, 0);
    for (uint32 l_Iter = 0; l_Iter < EQUIPMENT_SLOT_END; ++l_Iter)
    {
        p_RecvData.readPackGUID(l_ItemsGuids[l_Iter]);
        p_RecvData >> l_ItemAppearances[l_Iter];
    }

    uint32 l_Enchant_0 = p_RecvData.read<uint32>();
    uint32 l_Enchant_1 = p_RecvData.read<uint32>();

    bool l_HasSpecIndex = p_RecvData.ReadBit();

    l_SetNameLen = p_RecvData.ReadBits(8);
    l_IconNameLen = 2 * p_RecvData.ReadBits(8);
    if (p_RecvData.ReadBit())
        ++l_IconNameLen;

    p_RecvData.FlushBits();

    if (l_HasSpecIndex)
        p_RecvData >> l_EquipmentSet.AssignedSpecIndex;

    std::string l_Name, l_IconName;
    l_Name = p_RecvData.ReadString(l_SetNameLen);
    l_IconName = p_RecvData.ReadString(l_IconNameLen);

    if (l_SetID >= MAX_EQUIPMENT_SET_INDEX)                    // client set slots amount
        return;

    if (l_Type > EquipmentSetType::TRANSMOG)
        return;

    for (uint32 l_Iter = 0; l_Iter < EQUIPMENT_SLOT_END; ++l_Iter)
    {
        if (!(l_IgnoreMask & (1 << l_Iter)))
        {
            if (l_Type == EquipmentSetType::EQUIPMENT)
            {
                l_ItemAppearances[l_Iter] = 0;

                Item* l_Item = m_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Iter);
                if (!l_Item || !l_ItemsGuids[l_Iter])                                    ///< Cheating check 1
                    continue;

                if (l_Item && l_Item->GetGUID() != l_ItemsGuids[l_Iter])                ///< Cheating check 2
                    continue;

                l_EquipmentSet.Items[l_Iter] = GUID_LOPART(l_ItemsGuids[l_Iter]);
            }
            else
            {
                l_ItemsGuids[l_Iter] = 0;

                if (l_ItemAppearances[l_Iter])
                {
                    if (sItemModifiedAppearanceStore.LookupEntry(l_ItemAppearances[l_Iter]) == nullptr)
                        continue;
                }

                /// NYI
            }
        }
        else
        {
            l_ItemsGuids[l_Iter] = 0;
            l_ItemAppearances[l_Iter] = 0;
        }
    }

    l_IgnoreMask &= 0x7FFFF;

    if (l_Type == EquipmentSetType::EQUIPMENT)
    {
        l_Enchant_0 = 0;
        l_Enchant_1 = 0;
    }
    else
    {
        auto l_ValidateIllusion = [this](uint32 p_EnchantID) -> bool
        {
            SpellItemEnchantmentEntry const* l_Illusion = sSpellItemEnchantmentStore.LookupEntry(p_EnchantID);
            if (!l_Illusion)
                return false;

            if (!l_Illusion->ItemVisual || !(l_Illusion->Flags & ENCHANTMENT_COLLECTABLE))
                return false;

            if (!m_Player->EvalPlayerCondition(l_Illusion->PlayerConditionID).first)
                return false;

            if (l_Illusion->m_ScalingClassRestricted > 0 && uint8(l_Illusion->m_ScalingClassRestricted) != m_Player->getClass())
                return false;

            return true;
        };

        if (l_Enchant_0 && !l_ValidateIllusion(l_Enchant_0))
            return;

        if (l_Enchant_1 && !l_ValidateIllusion(l_Enchant_1))
            return;
    }

    l_EquipmentSet.Type         = l_Type;
    l_EquipmentSet.IgnoreMask   = l_IgnoreMask;
    l_EquipmentSet.Guid         = l_SetGuid;
    l_EquipmentSet.Name         = l_Name;
    l_EquipmentSet.IconName     = l_IconName;
    l_EquipmentSet.state        = EQUIPMENT_SET_NEW;

    m_Player->SetEquipmentSet(l_SetID, l_EquipmentSet);
}

void WorldSession::HandleEquipmentSetDelete(WorldPacket& p_RecvData)
{
    uint64 l_SetGuid = p_RecvData.read<uint64>();
    m_Player->DeleteEquipmentSet(l_SetGuid);
}

void WorldSession::HandleEquipmentSetUse(WorldPacket& p_RecvData)
{
    uint8 l_ItemCount = p_RecvData.ReadBits(2);

    std::vector<uint8> l_ItemBags(l_ItemCount, 0);
    std::vector<uint8> l_ItemSlots(l_ItemCount, 0);
    for (uint8 l_Iter = 0; l_Iter < l_ItemCount; ++l_Iter)
    {
        p_RecvData >> l_ItemBags[l_Iter];
        p_RecvData >> l_ItemSlots[l_Iter];
    }

    std::vector<uint64> l_ItemGuids(EQUIPMENT_SLOT_END, 0);
    std::vector<uint8>  l_ItemBag(EQUIPMENT_SLOT_END, 0);
    std::vector<uint8>  l_ItemSlot(EQUIPMENT_SLOT_END, 0);
    for (uint8 l_Iter = 0; l_Iter < EQUIPMENT_SLOT_END; ++l_Iter)
    {
        p_RecvData.readPackGUID(l_ItemGuids[l_Iter]);
        p_RecvData >> l_ItemSlot[l_Iter];
        p_RecvData >> l_ItemBag[l_Iter];
    }

    uint64 l_SetGuid = p_RecvData.read<uint64>();

    EquipmentSlots l_StartSlot = m_Player->isInCombat() ? EQUIPMENT_SLOT_MAINHAND : EQUIPMENT_SLOT_START;

    bool l_CanEquipGear = true;

    if (m_Player->GetMap() && m_Player->GetMap()->IsChallengeMode())
    {
        l_CanEquipGear = !(m_Player->HasAura(ChallengeSpells::ChallengerGrievousWound) || m_Player->HasAura(ChallengeSpells::ChallengerBurst));

        if (!l_CanEquipGear)
            m_Player->SendEquipError(InventoryResult::EQUIP_ERR_CLIENT_LOCKED_OUT, nullptr, nullptr);
    }

    for (uint32 l_Iter = 0; l_Iter < EQUIPMENT_SLOT_END && l_CanEquipGear; ++l_Iter)
    {
        if (l_Iter == EQUIPMENT_SLOT_RANGED)
            continue;

        if (l_Iter < uint32(l_StartSlot))
            continue;

        // check if item slot is set to "ignored" (raw value == 1), must not be unequipped then
        if (l_ItemGuids[l_Iter] == 1)
            continue;

        Item* l_Item = m_Player->GetItemByGuid(l_ItemGuids[l_Iter]);

        uint16 l_DestPos = l_Iter | (INVENTORY_SLOT_BAG_0 << 8);

        if (!l_Item)
        {
            Item* l_OtherItem = m_Player->GetItemByPos(INVENTORY_SLOT_BAG_0, l_Iter);
            if (!l_OtherItem)
                continue;

            ItemPosCountVec l_Dest;
            InventoryResult l_Result = m_Player->CanStoreItem(NULL_BAG, NULL_SLOT, l_Dest, l_OtherItem, false);
            if (l_Result == EQUIP_ERR_OK)
            {
                m_Player->RemoveItem(INVENTORY_SLOT_BAG_0, l_Iter, true);
                m_Player->StoreItem(l_Dest, l_OtherItem, true);
            }
            else
                m_Player->SendEquipError(l_Result, l_OtherItem, NULL);

            continue;
        }

        if (l_Item->GetPos() == l_DestPos)
            continue;

        /// Fury artifact should always be main-hand, otherwise the whole system goes apeshit, because weapons are same type, not main - off hand
        Item* l_Item1 = m_Player->GetItemByPos(l_Item->GetPos());
        Item* l_Item2 = m_Player->GetItemByPos(l_DestPos);

        uint16 l_Source = l_Item->GetPos();
        uint16 l_Dest   = l_DestPos;

        if (l_Item1 && l_Item1->GetEntry() == 128908)
        {
            uint8 l_DestSlot = EQUIPMENT_SLOT_MAINHAND;
            l_Dest = ((INVENTORY_SLOT_BAG_0 << 8) | l_DestSlot);
        }
        if (l_Item2 && l_Item2->GetEntry() == 128908)
        {
            if (l_Item->GetBagSlot() == INVENTORY_SLOT_BAG_0 || l_Item->GetSlot() == EQUIPMENT_SLOT_OFFHAND)
            {
                uint8 l_SourceSlot = EQUIPMENT_SLOT_MAINHAND;
                l_Source = ((l_Item->GetBagSlot() << 8) | l_SourceSlot);
            }
        }

        if (!m_Player->IsValidPos(l_Source, true))
        {
            m_Player->SendEquipError(EQUIP_ERR_ITEM_NOT_FOUND, NULL, NULL);
            continue;
        }

        if (!m_Player->IsValidPos(l_Dest, true))
        {
            m_Player->SendEquipError(EQUIP_ERR_WRONG_SLOT, NULL, NULL);
            continue;
        }

        m_Player->SwapItem(l_Source, l_Dest);
    }

    WorldPacket l_Data(SMSG_USE_EQUIPMENT_SET_RESULT);
    l_Data << uint64(l_SetGuid);
    l_Data << uint8(0);   ///< Reason 4 - equipment swap failed - inventory is full
    SendPacket(&l_Data);
}

#ifndef CROSS
void WorldSession::HandleCharRaceOrFactionChange(WorldPacket& p_Packet)
{
    bool l_FactionChange;

    uint8  l_SexID;
    uint8  l_RaceID;
    uint8  l_SkinID;
    uint8  l_HairColor;
    uint8  l_HairStyleID;
    uint8  l_FacialHairStyleID;
    uint8  l_FaceID;

    uint64      l_Guid;
    std::string l_Name;
    size_t      l_NameSize;

    l_FactionChange = p_Packet.ReadBit();
    l_NameSize = p_Packet.ReadBits(6);

    p_Packet.ResetBitReading();

    p_Packet.readPackGUID(l_Guid);
    p_Packet >> l_SexID;
    p_Packet >> l_RaceID;
    p_Packet >> l_SkinID;
    p_Packet >> l_HairColor;
    p_Packet >> l_HairStyleID;
    p_Packet >> l_FacialHairStyleID;
    p_Packet >> l_FaceID;

    std::array<uint8, PLAYER_CUSTOM_DISPLAY_SIZE> l_CustomDisplay;
    for (uint8 l_I = 0; l_I < PLAYER_CUSTOM_DISPLAY_SIZE; ++l_I)
        p_Packet >> l_CustomDisplay[l_I];

    l_Name = p_Packet.ReadString(l_NameSize);

    uint32 l_LowGuid = GUID_LOPART(l_Guid);

    // get the players old (at this moment current) race
    CharacterInfo const* l_NameData = sWorld->GetCharacterInfo(l_LowGuid);
    if (!l_NameData)
    {
        SendCharFactionChange(ResponseCodes::CHAR_CREATE_ERROR, l_Guid);
        return;
    }

    PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_CHAR_CLASS_LVL_AT_LOGIN);
    l_Statement->setUInt32(0, l_LowGuid);
    PreparedQueryResult l_QueryResult = CharacterDatabase.Query(l_Statement);

    if (!l_QueryResult)
    {
        SendCharFactionChange(ResponseCodes::CHAR_CREATE_ERROR, l_Guid);
        return;
    }

    Field* l_Fields         = l_QueryResult->Fetch();

    uint8  l_OldRace        = l_Fields[0].GetUInt8();
    uint32 l_PlayerClass    = uint32(l_Fields[1].GetUInt8());
    uint32 l_Level          = uint32(l_Fields[2].GetUInt8());
    uint32 l_AtLoginFlag    = l_Fields[3].GetUInt16();
    auto   l_KnownTitlesStr = l_Fields[4].GetCString();
    uint32 l_PlayerBytes    = l_Fields[5].GetUInt32();
    uint32 l_PlayerBytes2   = l_Fields[6].GetUInt32(); ///< L_playerbyte2 is never read 01/18/16

    // Search each faction is targeted
    BattlegroundTeamId l_OldTeam = BG_TEAM_ALLIANCE;
    switch (l_OldRace)
    {
        case RACE_ORC:
        case RACE_GOBLIN:
        case RACE_TAUREN:
        case RACE_UNDEAD_PLAYER:
        case RACE_TROLL:
        case RACE_BLOODELF:
        case RACE_PANDAREN_HORDE:
        case RACE_NIGHTBORNE:
        case RACE_HIGHMOUNTAIN_TAUREN:
            l_OldTeam = BG_TEAM_HORDE;
            break;
        default:
            break;
    }

    // Search each faction is targeted
    BattlegroundTeamId l_Team = BG_TEAM_ALLIANCE;
    switch (l_RaceID)
    {
        case RACE_ORC:
        case RACE_GOBLIN:
        case RACE_TAUREN:
        case RACE_UNDEAD_PLAYER:
        case RACE_TROLL:
        case RACE_BLOODELF:
        case RACE_PANDAREN_HORDE:
        case RACE_NIGHTBORNE:
        case RACE_HIGHMOUNTAIN_TAUREN:
            l_Team = BG_TEAM_HORDE;
            break;
        default:
            break;
    }

    uint32 l_UsedLoginFlag = l_AtLoginFlag;

    // We need to correct race when it's pandaren
    // Because client always send neutral ID
    if (l_RaceID == RACE_PANDAREN_NEUTRAL)
    {
        if (l_AtLoginFlag & AT_LOGIN_CHANGE_RACE)
            l_Team = l_OldTeam;
        else
            l_Team = l_OldTeam == BG_TEAM_ALLIANCE ? BG_TEAM_HORDE : BG_TEAM_ALLIANCE;
    }

    if (l_RaceID == RACE_PANDAREN_NEUTRAL)
    {
        if (l_OldTeam == l_Team)
            l_RaceID = l_OldTeam == BG_TEAM_ALLIANCE ? RACE_PANDAREN_ALLIANCE : RACE_PANDAREN_HORDE;
        else
            l_RaceID = l_OldTeam == BG_TEAM_ALLIANCE ? RACE_PANDAREN_HORDE : RACE_PANDAREN_ALLIANCE;
    }

    // Not really necessary because changing race does not include faction change
    // But faction change can include race change
    if (l_OldTeam != l_Team)
        l_UsedLoginFlag = AT_LOGIN_CHANGE_FACTION;

    if (!sObjectMgr->GetPlayerInfo(l_RaceID, l_PlayerClass))
    {
        SendCharFactionChange(ResponseCodes::CHAR_CREATE_ERROR, l_Guid);
        return;
    }

    if (!(l_AtLoginFlag & l_UsedLoginFlag))
    {
        SendCharFactionChange(ResponseCodes::CHAR_CREATE_ERROR, l_Guid);
        return;
    }

    if (AccountMgr::IsPlayerAccount(GetSecurity()))
    {
        uint64 l_RaceMaskDisabled = sWorld->getIntConfig(CONFIG_CHARACTER_CREATING_DISABLED_RACEMASK);
        if ((1LL << (l_RaceID - 1)) & l_RaceMaskDisabled)
        {
            SendCharFactionChange(ResponseCodes::CHAR_CREATE_ERROR, l_Guid);
            return;
        }
    }

    // prevent character rename to invalid name
    if (!normalizePlayerName(l_Name))
    {
        SendCharFactionChange(ResponseCodes::CHAR_NAME_NO_NAME, l_Guid);
        return;
    }

    uint8 l_Result = ObjectMgr::CheckPlayerName(l_Name, true);
    if (l_Result != CHAR_NAME_SUCCESS)
    {
        SendCharFactionChange(ResponseCodes(l_Result), l_Guid);
        return;
    }

    // check name limitations
    if (AccountMgr::IsPlayerAccount(GetSecurity()) && sObjectMgr->IsReservedName(l_Name))
    {
        SendCharFactionChange(ResponseCodes::CHAR_NAME_RESERVED, l_Guid);
        return;
    }

    // character with this name already exist
    if (uint64 l_NewGuid = sWorld->GetCharacterGuidByName(l_Name))
    {
        if (l_NewGuid != l_Guid)
        {
            SendCharFactionChange(ResponseCodes::CHAR_CREATE_NAME_IN_USE, l_Guid);
            return;
        }
    }

    CharacterDatabase.EscapeString(l_Name);
    Player::Customize(l_Guid, l_SexID, l_SkinID, l_FaceID, l_HairStyleID, l_HairColor, l_FacialHairStyleID, l_CustomDisplay);
    SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_FACTION_OR_RACE);
    l_Statement->setString(0, l_Name);
    l_Statement->setUInt8 (1, l_RaceID);
    l_Statement->setUInt16(2, l_UsedLoginFlag);
    l_Statement->setUInt32(3, l_LowGuid);
    l_Transaction->Append(l_Statement);

    l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_FACTION_CHANGE_LOG);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Statement->setUInt32(1, GetAccountId());
    l_Statement->setUInt8 (2, l_OldRace);
    l_Statement->setUInt8 (3, l_RaceID);
    LogDatabase.Execute(l_Statement);

    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_DECLINED_NAME);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Transaction->Append(l_Statement);

    // CHECK PTR
    if (l_NameData)
    {
        l_Statement = LogDatabase.GetPreparedStatement(LOG_INSERT_NAME_LOG);
        l_Statement->setUInt32(0, l_LowGuid);
        l_Statement->setString(1, l_NameData->Name);
        l_Statement->setString(2, l_Name);
        LogDatabase.Execute(l_Statement);
    }

    sWorld->UpdateCharacterInfo(GUID_LOPART(l_Guid), l_Name, l_SexID, l_RaceID);

    // Switch Languages
    // delete all languages first
    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SKILL_LANGUAGES);
    l_Statement->setUInt32(0, l_LowGuid);
    l_Transaction->Append(l_Statement);

    // Now add them back
    l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SKILL_LANGUAGE);
    l_Statement->setUInt32(0, l_LowGuid);

    // Faction specific languages
    if (l_Team == BG_TEAM_HORDE)
        l_Statement->setUInt16(1, 109);
    else
        l_Statement->setUInt16(1, 98);

    l_Transaction->Append(l_Statement);

    // Race specific languages
    if (l_RaceID != RACE_ORC && l_RaceID != RACE_HUMAN)
    {
        l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_CHAR_SKILL_LANGUAGE);
        l_Statement->setUInt32(0, l_LowGuid);

        switch (l_RaceID)
        {
            case RACE_DWARF:
                l_Statement->setUInt16(1, SKILL_LANG_DWARVEN);
                break;
            case RACE_DRAENEI:
                l_Statement->setUInt16(1, SKILL_LANG_DRAENEI);
                break;
            case RACE_GNOME:
                l_Statement->setUInt16(1, SKILL_LANG_GNOMISH);
                break;
            case RACE_NIGHTELF:
                l_Statement->setUInt16(1, SKILL_LANG_DARNASSIAN);
                break;
            case RACE_WORGEN:
                l_Statement->setUInt16(1, SKILL_LANG_GILNEAN);
                break;
            case RACE_UNDEAD_PLAYER:
                l_Statement->setUInt16(1, SKILL_LANG_FORSAKEN);
                break;
            case RACE_TAUREN:
                l_Statement->setUInt16(1, SKILL_LANG_TAURAHE);
                break;
            case RACE_TROLL:
                l_Statement->setUInt16(1, SKILL_LANG_TROLL);
                break;
            case RACE_BLOODELF:
                l_Statement->setUInt16(1, SKILL_LANG_THALASSIAN);
                break;
            case RACE_GOBLIN:
                l_Statement->setUInt16(1, SKILL_LANG_GOBLIN);
                break;
            case RACE_PANDAREN_ALLIANCE:
                l_Statement->setUInt16(1, SKILL_LANG_PANDAREN_ALLIANCE);
                break;
            case RACE_PANDAREN_HORDE:
                l_Statement->setUInt16(1, SKILL_LANG_PANDAREN_HORDE);
                break;
            case RACE_PANDAREN_NEUTRAL:
                l_Statement->setUInt16(1, SKILL_LANG_PANDAREN_NEUTRAL);
                break;
            case RACE_NIGHTBORNE:
                l_Statement->setUInt16(1, SKILL_LANGUAGE_SHALASSIAN);
                break;
            case RACE_HIGHMOUNTAIN_TAUREN:
                l_Statement->setUInt16(1, SKILL_LANG_TAURAHE);
                break;
            case RACE_VOID_ELF:
                l_Statement->setUInt16(1, SKILL_LANG_THALASSIAN);
                break;
            case RACE_LIGHTFORGED_DRAENEI:
                l_Statement->setUInt16(1, SKILL_LANG_DRAENEI);
                break;
        }

        l_Transaction->Append(l_Statement);
    }

    if (l_OldTeam != l_Team)
    {
        // Delete all Flypaths
        PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_TAXI_PATH);
        l_Statement->setUInt32(0, l_LowGuid);
        l_Transaction->Append(l_Statement);

        if (l_Level > 7)
        {
            // Update Taxi path
            // this doesn't seem to be 100% blizzlike... but it can't really be helped.
            std::ostringstream taximaskstream;
            TaxiMask const& factionMask = l_Team == BG_TEAM_HORDE ? sHordeTaxiNodesMask : sAllianceTaxiNodesMask;
            for (uint32 i = 0; i < TaxiMaskSize; ++i)
            {
                // i = (315 - 1) / 8 = 39
                // m = 1 << ((315 - 1) % 8) = 4
                uint8 deathKnightExtraNode = l_PlayerClass != CLASS_DEATH_KNIGHT || i != 39 ? 0 : 4;
                taximaskstream << uint32(factionMask[i] | deathKnightExtraNode) << ' ';
            }

            l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_TAXIMASK);
            l_Statement->setString(0, taximaskstream.str());
            l_Statement->setUInt64(1, l_LowGuid);
            l_Transaction->Append(l_Statement);
        }

        // Delete all current quests
        l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_QUESTSTATUS);
        l_Statement->setUInt32(0, GUID_LOPART(l_Guid));
        l_Transaction->Append(l_Statement);

        // Delete record of the faction old completed quests
        /// @TODO: Rewrite it (do we really need it tho?) atm it do a very long request that crash the realm
        /*{
            std::ostringstream l_Quests;
            ObjectMgr::QuestMap const& l_QuestTemplates = sObjectMgr->GetQuestTemplates();
            for (ObjectMgr::QuestMap::const_iterator iter = l_QuestTemplates.begin(); iter != l_QuestTemplates.end(); ++iter)
            {
                Quest *l_Quest = iter->second;
                int64 l_RequiredRaces = l_Quest->GetRequiredRaces();
                if (l_Team == BG_TEAM_ALLIANCE)
                {
                    if (l_RequiredRaces & (int64)RACEMASK_ALLIANCE)
                    {
                        l_Quests << uint32(l_Quest->GetQuestId());
                        l_Quests << ',';
                    }
                }
                else // if (team == BG_TEAM_HORDE)
                {
                    if (l_RequiredRaces & (int64)RACEMASK_HORDE)
                    {
                        l_Quests << uint32(l_Quest->GetQuestId());
                        l_Quests << ',';
                    }
                }
            }

            std::string l_QuestsString = l_Quests.str();
            l_QuestsString = l_QuestsString.substr(0, l_QuestsString.length() - 1);

            if (!l_QuestsString.empty())
                l_Transaction->PAppend("DELETE FROM `character_queststatus_rewarded` WHERE guid='%u' AND quest IN (%s)", l_LowGuid, l_QuestsString.c_str());
        }*/

        if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GUILD))
        {
            // Reset guild
            PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_SEL_GUILD_MEMBER);

            l_Statement->setUInt32(0, l_LowGuid);

            PreparedQueryResult l_QueryResult = CharacterDatabase.Query(l_Statement);
            if (l_QueryResult)
                if (Guild* l_Guild = sGuildMgr->GetGuildById((l_QueryResult->Fetch()[0]).GetUInt32()))
                    l_Guild->DeleteMember(MAKE_NEW_GUID(l_LowGuid, 0, HIGHGUID_PLAYER), false, false, true);
        }

        if (!sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_ADD_FRIEND))
        {
            // Delete Friend List
            PreparedStatement* l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SOCIAL_BY_ACCOUNT_ID);
            l_Statement->setUInt32(0, GetAccountId());
            l_Transaction->Append(l_Statement);

            l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_SOCIAL_BY_FRIEND);
            l_Statement->setUInt32(0, l_LowGuid);
            l_Transaction->Append(l_Statement);

            sLog->outAshran("removing friends in WorldSession::HandleCharRaceOrFactionChange for account %u", _accountId);
        }

        // Reset homebind and position
        l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_DEL_PLAYER_HOMEBIND);
        l_Statement->setUInt32(0, l_LowGuid);
        l_Transaction->Append(l_Statement);

        l_Statement = CharacterDatabase.GetPreparedStatement(CHAR_INS_PLAYER_HOMEBIND);
        l_Statement->setUInt32(0, l_LowGuid);
        if (l_Team == BG_TEAM_ALLIANCE)
        {
            l_Statement->setUInt16(1, 0);
            l_Statement->setUInt16(2, 1519);
            l_Statement->setFloat (3, -8866.19f);
            l_Statement->setFloat (4, 671.16f);
            l_Statement->setFloat (5, 97.9034f);
            Player::SavePositionInDB(0, -8866.19f, 671.16f, 97.9034f, 0.0f, 1519, l_LowGuid);
        }
        else
        {
            l_Statement->setUInt16(1, 1);
            l_Statement->setUInt16(2, 1637);
            l_Statement->setFloat (3, 1633.33f);
            l_Statement->setFloat (4, -4439.11f);
            l_Statement->setFloat (5, 17.05f);
            Player::SavePositionInDB(1, 1633.33f, -4439.11f, 17.05f, 0.0f, 1637, l_LowGuid);
        }

        l_Transaction->Append(l_Statement);

        Player::GenerateFactionConversionQuery(l_Transaction, l_LowGuid, l_Team, l_KnownTitlesStr);
    }

    CharacterDatabase.CommitTransaction(l_Transaction);

    std::string l_IPString = GetRemoteAddress();

    WorldPacket l_Data(SMSG_CHAR_FACTION_CHANGE_RESULT, 1 + 8 + (l_Name.size() + 1) + 1 + 1 + 1 + 1 + 1 + 1 + 1);
    l_Data << uint8(RESPONSE_SUCCESS);      ///< Result
    l_Data.appendPackGUID(l_Guid);          ///< Guid
    l_Data.WriteBit(true);                  ///< l_HasDisplayInfo
    l_Data.FlushBits();

    /// - HasDisplayInfo block
    {
        l_Data.WriteBits(l_NameSize, 6);
        l_Data << uint8(l_SexID);
        l_Data << uint8(l_SkinID);
        l_Data << uint8(l_HairColor);
        l_Data << uint8(l_HairStyleID);
        l_Data << uint8(l_FacialHairStyleID);
        l_Data << uint8(l_FaceID);
        l_Data << uint8(l_RaceID);

        for (uint8 l_I = 0; l_I < PLAYER_CUSTOM_DISPLAY_SIZE; ++l_I)
            l_Data << l_CustomDisplay[l_I];

        l_Data.WriteString(l_Name);
    }

    SendPacket(&l_Data);
}

void WorldSession::HandleRandomizeCharNameOpcode(WorldPacket& recvData)
{
    uint8 gender, race;

    recvData >> race;
    recvData >> gender;

    if (!Player::IsValidRace(race))
    {
        sLog->outError(LOG_FILTER_GENERAL, "Invalid race (%u) sent by accountId: %u", race, GetAccountId());
        return;
    }

    if (!Player::IsValidGender(gender))
    {
        sLog->outError(LOG_FILTER_GENERAL, "Invalid gender (%u) sent by accountId: %u", gender, GetAccountId());
        return;
    }

    std::string const* name = GetRandomCharacterName(race, gender);
    if (!name)
        return;

    WorldPacket data(SMSG_GENERATE_RANDOM_CHARACTER_NAME_RESULT, 10);
    data.WriteBit(0);               ///< Success
    data.WriteBits(name->size(), 6);
    data.WriteString(name->c_str());
    SendPacket(&data);
}

void WorldSession::HandleReorderCharacters(WorldPacket& p_Packet)
{
    uint32 l_CharactersCount = p_Packet.ReadBits(9);

    std::vector<uint64> l_Guids(l_CharactersCount);
    uint8 l_Position;

    p_Packet.FlushBits();

    SQLTransaction l_Transaction = CharacterDatabase.BeginTransaction();

    for (uint8 l_I = 0; l_I < l_CharactersCount; ++l_I)
    {
        p_Packet.readPackGUID(l_Guids[l_I]);
        p_Packet >> l_Position;

        PreparedStatement* l_Stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_CHAR_LIST_SLOT);
        l_Stmt->setUInt8(0, l_Position);
        l_Stmt->setUInt32(1, GUID_LOPART(l_Guids[l_I]));
        l_Transaction->Append(l_Stmt);
    }

    CharacterDatabase.CommitTransaction(l_Transaction);
}

void WorldSession::HandleAuthSessionOpcode(WorldPacket & recvPacket)
{
}
#endif

void WorldSession::HandleSuspendToken(WorldPacket& p_RecvData)
{
    uint32 l_Token = p_RecvData.read<uint32>();

    m_Player->m_tokenCounter = l_Token;
    GetPlayer()->SendResumeToken(l_Token);
}

void WorldSession::AbortLogin(WorldPackets::Character::LoginFailureReason reason)
{
    if (!PlayerLoading() || GetPlayer())
    {
        KickPlayer();
        return;
    }

    m_playerLoading = 0;
    SendPacket(WorldPackets::Character::CharacterLoginFailed(reason).Write());
}
