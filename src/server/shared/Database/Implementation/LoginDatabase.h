////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _LOGINDATABASE_H
#define _LOGINDATABASE_H

#include "DatabaseWorkerPool.h"
#include "MySQLConnection.h"

enum LoginDatabaseStatements
{
    /*  Naming standard for defines:
        {DB}_{SEL/INS/UPD/DEL/REP}_{Summary of data changed}
        When updating more than one field, consider looking at the calling function
        name for a suiting suffix.
    */

    LOGIN_SEL_REALMLIST = 1,
    LOGIN_DEL_EXPIRED_IP_BANS,
    LOGIN_UPD_EXPIRED_ACCOUNT_BANS,
    LOGIN_SEL_IP_BANNED,
    LOGIN_SEL_ACCOUNT_BANNED,
    LOGIN_SEL_ACCOUNT_BANNED_ALL,
    LOGIN_SEL_ACCOUNT_BANNED_BY_USERNAME,
    LOGIN_DEL_ACCOUNT_BANNED,
    LOGIN_SEL_SESSIONKEY,
    LOGIN_UPD_VS,
    LOGIN_UPD_LOGONPROOF,
    LOGIN_SEL_LOGONCHALLENGE,
    LOGIN_UPD_FAILEDLOGINS,
    LOGIN_SEL_FAILEDLOGINS,
    LOGIN_SEL_ACCOUNT_ID_BY_NAME,
    LOGIN_SEL_ACCOUNT_LIST_BY_NAME,
    LOGIN_SEL_ACCOUNT_INFO_BY_NAME,
    LOGIN_SEL_ACCOUNT_LIST_BY_EMAIL,
    LOGIN_SEL_NUM_CHARS_ON_REALM,
    LOGIN_SEL_ACCOUNT_BY_IP,
    LOGIN_INS_IP_BANNED,
    LOGIN_DEL_IP_NOT_BANNED,
    LOGIN_SEL_IP_BANNED_ALL,
    LOGIN_SEL_IP_BANNED_BY_IP,
    LOGIN_SEL_ACCOUNT_BY_ID,
    LOGIN_INS_ACCOUNT_BANNED,
    LOGIN_UPD_ACCOUNT_NOT_BANNED,
    LOGIN_SEL_LAST_BANNED_DATE,
    LOGIN_SEL_ACCOUNT_ALWAYS_BANNED,
    LOGIN_SEL_ACCOUNT_BANNED_PERMANENT,
    LOGIN_DEL_REALM_CHARACTERS_BY_REALM,
    LOGIN_DEL_REALM_CHARACTERS,
    LOGIN_INS_REALM_CHARACTERS,
    LOGIN_UPD_REALM_CHARACTERS,
    LOGIN_SEL_SUM_REALM_CHARACTERS,
    LOGIN_INS_ACCOUNT,
    LOGIN_INS_REALM_CHARACTERS_INIT,
    LOGIN_UPD_EXPANSION,
    LOGIN_UPD_ACCOUNT_LOCK,
    LOGIN_INS_LOG,
    LOGIN_UPD_USERNAME,
    LOGIN_UPD_PASSWORD,
    LOGIN_UPD_MUTE_TIME,
    LOGIN_UPD_LAST_IP,
    LOGIN_UPD_ACCOUNT_ONLINE,
    LOGIN_UPD_UPTIME_PLAYERS,
    LOGIN_DEL_OLD_LOGS,
    LOGIN_DEL_ACCOUNT_ACCESS,
    LOGIN_DEL_ACCOUNT_ACCESS_BY_REALM,
    LOGIN_INS_ACCOUNT_ACCESS,
    LOGIN_GET_ACCOUNT_ID_BY_USERNAME,
    LOGIN_GET_ACCOUNT_ACCESS_GMLEVEL,
    LOGIN_GET_GMLEVEL_BY_REALMID,
    LOGIN_GET_USERNAME_BY_ID,
    LOGIN_SEL_CHECK_PASSWORD,
    LOGIN_SEL_CHECK_PASSWORD_BY_NAME,
    LOGIN_SEL_PINFO,
    LOGIN_SEL_PINFO_BANS,
    LOGIN_SEL_GM_ACCOUNTS,
    LOGIN_SEL_ACCOUNT_INFO,
    LOGIN_SEL_ACCOUNT_ACCESS_GMLEVEL_TEST,
    LOGIN_SEL_ACCOUNT_ACCESS,
    LOGIN_SEL_ACCOUNT_RECRUITER,
    LOGIN_SEL_BANS,
    LOGIN_SEL_ACCOUNT_WHOIS,
    LOGIN_SEL_REALMLIST_SECURITY_LEVEL,
    LOGIN_DEL_ACCOUNT,
    LOGIN_DEL_LAST_PLAYED_CHARACTER,
    LOGIN_INS_LAST_PLAYED_CHARACTER,

    //////////////////////////////////////////////////////////////////////////
    /// ToyBox
    LOGIN_SEL_ACCOUNT_TOYS,
    LOGIN_INS_ACCOUNT_TOYS,
    LOGIN_UPD_TOY_FAVORITE,
    //////////////////////////////////////////////////////////////////////////

    LOGIN_INS_CHAR_SPELL,
    LOGIN_SEL_CHARACTER_SPELL,
    LOGIN_DEL_CHAR_SPELL_BY_SPELL,
    LOGIN_DEL_CHAR_SPELL,

    LOGIN_UPD_ACCOUNT_PREMIUM,
    LOGIN_SELECT_PDUMP,

    LOGIN_SEL_PETBATTLE_ACCOUNT,
    LOGIN_REP_PETBATTLE,
    LOGIN_INS_PETBATTLE,

    //////////////////////////////////////////////////////////////////////////
    /// Transfers
    LOGIN_SEL_TRANSFERS_DUMP,
    LOGIN_SEL_TRANSFERS_LOAD,
    LOGIN_UPD_TRANSFER_PDUMP,
    LOGIN_SEL_TRANSFERS_EXP_LOAD,
    LOGIN_UPD_TRANSFER_EXP,

    //////////////////////////////////////////////////////////////////////////

    LOGIN_SEL_BATTLEPAY_POINTS,

    //////////////////////////////////////////////////////////////////////////
    /// Heirloom Colleciton
    LOGIN_SEL_HEIRLOOM_COLLECTION,
    LOGIN_INS_HEIRLOOM,
    LOGIN_UPD_HEILOOM_FLAGS,
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Wardrobe
    LOGIN_SEL_ACCOUNT_WARDROBES,
    LOGIN_INS_ACCOUNT_WARDROBES,
    LOGIN_SEL_ACCOUNT_WARDROBE_FAVORITES,
    LOGIN_INS_ACCOUNT_WARDROBE_FAVORITE,
    LOGIN_DEL_ACCOUNT_WARDROBE_FAVORITE,
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// User Reporting
    LOGIN_UPD_USER_REPORTING_STEP,
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Services
    LOGIN_REMOVE_ACCOUNT_SERVICE,
    LOGIN_SET_ACCOUNT_SERVICE,
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    /// Achievement
    //////////////////////////////////////////////////////////////////////////
    LOGIN_SEL_ACCOUNT_ACHIEVEMENTS,
    LOGIN_SEL_ACCOUNT_CRITERIAPROGRESS,
    LOGIN_DEL_ACCOUNT_ACHIEVEMENT_PROGRESS_BY_CRITERIA,
    LOGIN_INS_ACCOUNT_ACHIEVEMENT_PROGRESS,
    LOGIN_DEL_ACCOUNT_ACHIEVEMENT_BY_ACHIEVEMENT,
    LOGIN_INS_ACCOUNT_ACHIEVEMENT,
    //////////////////////////////////////////////////////////////////////////

    LOGIN_RPL_CHARACTER_RENDERER_QUEUE,

    //////////////////////////////////////////////////////////////////////////
    /// Note
    LOGIN_INS_ACCOUNT_NOTE,
    LOGIN_SEL_ACCOUNT_NOTE,
    LOGIN_SEL_LAST_ACCOUNT_NOTE,
    /////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////
    /// Recruit a friend
    /////////////////////////////////////////////
    LOGIN_SEL_RAF_REQUEST_MAILS,
    LOGIN_INS_RAF_REQUEST,
    LOGIN_SEL_RAF_ACC,
    LOGIN_INS_RAF_ACC,
    LOGIN_UPD_RAF_CHARACTER,
    LOGIN_SEL_RAF_ACC_BONUSES,
    LOGIN_REP_RAF_ACC_BONUSES,
    LOGIN_UPD_RAF_ACC_BONUSES_REWARD,
    /////////////////////////////////////////////

    LOGIN_SEL_ACTIVITY,

    LOGIN_SEL_ACC_LOYALTY,
    LOGIN_REP_ACC_LOYALTY,
    LOGIN_SEL_ACC_LOYALTY_EVENT,
    LOGIN_DEL_ACC_LOYALTY_EVENT,
    LOGIN_REP_ACC_LOYALTY_EVENT,

    LOGIN_SCAN_LAST_ACCOUNT_LOG,
    LOGIN_SEL_ACCS_SECURITY,

    LOGIN_INS_REAL_UPDATE_TIME,

    LOGIN_SEL_COSMETIC,
    LOGIN_INS_COSMETIC,

    LOGIN_SEL_ACCOUNT_QUEST_REWARDED,
    LOGIN_INS_ACCOUNT_QUEST_REWARDED,
    LOGIN_DEL_ACCOUNT_QUEST_REWARDED,

    LOGIN_SEL_AUTH_SESSION_ACCOUNT,
    LOGIN_SEL_AUTH_SESSION_GMLEVEL,
    LOGIN_SEL_AUTH_SESSION_BANNED,
    LOGIN_SEL_AUTH_SESSION_PREMIUM,
    LOGIN_SEL_AUTH_SESSION_TRUSTED,

    LOGIN_SEL_ACCOUNT_SESSIONKEY,

    MAX_LOGINDATABASE_STATEMENTS
};

class LoginDatabaseConnection : public MySQLConnection
{
public:
    typedef LoginDatabaseStatements Statements;

    //- Constructors for sync and async connections
    LoginDatabaseConnection(MySQLConnectionInfo& connInfo) : MySQLConnection(connInfo) {}
    LoginDatabaseConnection(moodycamel::ConcurrentQueue<SQLOperation*>* q, MySQLConnectionInfo& connInfo) : MySQLConnection(q, connInfo) {}

    //- Loads database type specific prepared statements
    void DoPrepareStatements() override;
};

typedef DatabaseWorkerPool<LoginDatabaseConnection> LoginDatabaseWorkerPool;

#endif
