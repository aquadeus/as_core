#ifdef CROSS
#ifndef __INTERREALM_DBH__
#define __INTERREALM_DBH__

#include "DatabaseEnv.h"
#include "DatabaseWorkerPool.h"

class InterRealmDatabaseConnection : public CharacterDatabaseConnection
{
public:
    //- Constructors for sync and async connections
    InterRealmDatabaseConnection(MySQLConnectionInfo& connInfo) : CharacterDatabaseConnection(connInfo) {}
    InterRealmDatabaseConnection(moodycamel::ConcurrentQueue<SQLOperation *> * q, MySQLConnectionInfo& connInfo) : CharacterDatabaseConnection(q, connInfo) {}
};
typedef DatabaseWorkerPool<InterRealmDatabaseConnection> InterRealmDatabasePool;

#endif
#endif