////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2014-2015 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////
#ifndef LOGDATABASE_H_INCLUDED
#define LOGDATABASE_H_INCLUDED

#include "DatabaseWorkerPool.h"
#include "MySQLConnection.h"

enum LogDatabaseStatements
{
    LOG_INSERT_MAP_DIFF_MESURE = 1,
    LOG_INSERT_LOG_MONEY,
    LOG_INSERT_LOOTBOX_LOG,
    LOG_INSERT_MAIL_LOG,
    LOG_INSERT_NAME_LOG,
    LOG_INSERT_GM_CHAT_LOG,
    LOG_INSERT_GM_LOG,
    LOG_INSERT_FACTION_CHANGE_LOG,
    LOG_INSERT_AUCTION_HOUSE_SELL_LOG,
    LOG_INSERT_ENCOUNTER_DAMAGE_LOG,
    LOG_INSERT_ENCOUNTER_GROUP_DUMP_LOG,
    LOG_INSERT_TIME_DIFF_LOG,

    LOG_INSERT_ITEM_GM_LOG,
    LOG_UPDATE_ITEM_GM_LOG,
    LOG_SELECT_ITEM_GM_LOG,

    MAX_LOGDATABASE_STATEMENTS,
};

class LogDatabaseConnection : public MySQLConnection
{
public:
    typedef LogDatabaseStatements Statements;

    /// Constructors for sync and async connections
    LogDatabaseConnection(MySQLConnectionInfo& p_ConnectionInfo) : MySQLConnection(p_ConnectionInfo) { }
    LogDatabaseConnection(moodycamel::ConcurrentQueue<SQLOperation*>* p_Queue, MySQLConnectionInfo& p_ConnectionInfo) : MySQLConnection(p_Queue, p_ConnectionInfo)  { }

    /// Loads database type specific prepared statements
    void DoPrepareStatements() override;
};

typedef DatabaseWorkerPool<LogDatabaseConnection> LogDatabaseWorkerPool;

#endif  ///< LOGDATABASE_H_INCLUDED
