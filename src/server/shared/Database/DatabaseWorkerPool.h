////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _DATABASEWORKERPOOL_H
#define _DATABASEWORKERPOOL_H

#include "Common.h"
#include "Callback.h"
#include "MySQLConnection.h"
#include "Transaction.h"
#include "DatabaseWorker.h"
#include "PreparedStatement.h"
#include "Log.h"
#include "QueryResult.h"
#include "QueryHolder.h"
#include "AdhocStatement.h"
#include "MSCallback.hpp"
#include "ProducerConsumerQueue.h"
#include "ConcurrentQueue.h"
#include "QueryCallbackManager.h"

#include <memory>

class PingOperation : public SQLOperation
{
    //! Operation for idle delaythreads
    bool Execute()
    {
        m_conn->Ping();
        return true;
    }
};

enum InternalIndex
{
    IDX_ASYNC,
    IDX_SYNCH,
    IDX_SIZE
};

template <class T>
class DatabaseWorkerPool
{
    public:
        /* Activity state */
        DatabaseWorkerPool() : _connectionInfo(nullptr), _queue(new moodycamel::ConcurrentQueue<SQLOperation*>(10000)),
            _async_threads(0), _synch_threads(0)
        {
            memset(_connectionCount, 0, sizeof(_connectionCount));
            _connections.resize(IDX_SIZE);

            WPFatal (mysql_thread_safe(), "Used MySQL library isn't thread-safe.");
        }

        ~DatabaseWorkerPool()
        {
        }


        void SetConnectionInfo(std::string const& infoString, uint8 const asyncThreads, uint8 const synchThreads)
        {
            _connectionInfo.reset(new MySQLConnectionInfo(infoString));

            _async_threads = asyncThreads;
            _synch_threads = synchThreads;
        }

        uint32 Open()
        {
            WPFatal(_connectionInfo.get(), "Connection info was not set!");

            sLog->outInfo(LOG_FILTER_SQL_DRIVER, "Opening DatabasePool '%s'. Asynchronous connections: %u, synchronous connections: %u.",
                GetDatabaseName(), _async_threads, _synch_threads);

            uint32 error = OpenConnections(IDX_ASYNC, _async_threads);

            if (error)
                return error;

            error = OpenConnections(IDX_SYNCH, _synch_threads);

            if (!error)
            {
                sLog->outInfo(LOG_FILTER_SQL_DRIVER, "DatabasePool '%s' opened successfully. %u total connections running.", GetDatabaseName(),
                    (_connectionCount[IDX_SYNCH] + _connectionCount[IDX_ASYNC]));
            }

            return error;
        }

        void Close()
        {
            sLog->outInfo(LOG_FILTER_SQL_DRIVER, "Closing down DatabasePool '%s'.", GetDatabaseName());

            for (uint8 i = 0; i < _connectionCount[IDX_ASYNC]; ++i)
            {
                T* t = _connections[IDX_ASYNC][i];
                t->Close();         //! Closes the actualy MySQL connection.
            }

            sLog->outInfo(LOG_FILTER_SQL_DRIVER, "Asynchronous connections on DatabasePool '%s' terminated. Proceeding with synchronous connections.",
                GetDatabaseName());

            //! Shut down the synchronous connections
            //! There's no need for locking the connection, because DatabaseWorkerPool<>::Close
            //! should only be called after any other thread tasks in the core have exited,
            //! meaning there can be no concurrent access at this point.
            for (uint8 i = 0; i < _connectionCount[IDX_SYNCH]; ++i)
                _connections[IDX_SYNCH][i]->Close();

            sLog->outInfo(LOG_FILTER_SQL_DRIVER, "All connections on DatabasePool '%s' closed.", GetDatabaseName());
        }

        //! Prepares all prepared statements
        bool PrepareStatements()
        {
            for (uint8 i = 0; i < IDX_SIZE; ++i)
                for (uint32 c = 0; c < _connectionCount[i]; ++c)
                {
                    T* t = _connections[i][c];
                    t->LockIfReady();
                    if (!t->PrepareStatements())
                    {
                        t->Unlock();
                        Close();
                        return false;
                    }
                    else
                        t->Unlock();
                }

            return true;
        }

        inline MySQLConnectionInfo const* GetConnectionInfo() const
        {
            return _connectionInfo.get();
        }

        /**
            Delayed one-way statement methods.
        */

        //! Enqueues a one-way SQL operation in string format that will be executed asynchronously.
        //! This method should only be used for queries that are only executed once, e.g during startup.
        void Execute(const char* sql)
        {
            if (!sql)
                return;

            BasicStatementTask* task = new BasicStatementTask(sql);
            Enqueue(task);
        }

        //! Enqueues a one-way SQL operation in string format -with variable args- that will be executed asynchronously.
        //! This method should only be used for queries that are only executed once, e.g during startup.
        void PExecute(const char* sql, ...)
        {
            if (!sql)
                return;

            va_list ap;
            char szQuery[MAX_QUERY_LEN];
            va_start(ap, sql);
            vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
            va_end(ap);

            Execute(szQuery);
        }

        //! Enqueues a one-way SQL operation in prepared statement format that will be executed asynchronously.
        //! Statement must be prepared with CONNECTION_ASYNC flag.
        void Execute(PreparedStatement* stmt)
        {
            PreparedStatementTask* task = new PreparedStatementTask(stmt);
            Enqueue(task);
        }

        /**
            Direct synchronous one-way statement methods.
        */

        //! Directly executes a one-way SQL operation in string format, that will block the calling thread until finished.
        //! This method should only be used for queries that are only executed once, e.g during startup.
        void DirectExecute(const char* sql)
        {
            if (!sql)
                return;

            T* t = GetFreeConnection();
            t->Execute(sql);
            t->Unlock();
        }

        //! Directly executes a one-way SQL operation in string format -with variable args-, that will block the calling thread until finished.
        //! This method should only be used for queries that are only executed once, e.g during startup.
        void DirectPExecute(const char* sql, ...)
        {
            if (!sql)
                return;

            va_list ap;
            char szQuery[MAX_QUERY_LEN];
            va_start(ap, sql);
            vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
            va_end(ap);

            return DirectExecute(szQuery);
        }

        //! Directly executes a one-way SQL operation in prepared statement format, that will block the calling thread until finished.
        //! Statement must be prepared with the CONNECTION_SYNCH flag.
        void DirectExecute(PreparedStatement* stmt)
        {
            T* t = GetFreeConnection();
            t->Execute(stmt);
            t->Unlock();

            //! Delete proxy-class. Not needed anymore
            delete stmt;
        }

        bool DirectExecuteWithReturn(PreparedStatement* stmt)
        {
            T* t = GetFreeConnection();
            bool result = t->Execute(stmt);
            t->Unlock();

            //! Delete proxy-class. Not needed anymore
            delete stmt;
            return result;
        }

        /**
            Synchronous query (with resultset) methods.
        */

        //! Directly executes an SQL query in string format that will block the calling thread until finished.
        //! Returns reference counted auto pointer, no need for manual memory management in upper level code.
        QueryResult Query(const char* sql, MySQLConnection* conn = NULL)
        {
            if (!conn)
                conn = GetFreeConnection();

            ResultSet* result = conn->Query(sql);
            conn->Unlock();
            if (!result || !result->GetRowCount())
            {
                delete result;
                return QueryResult(NULL);
            }
            result->NextRow();
            return QueryResult(result);
        }

        //! Directly executes an SQL query in string format -with variable args- that will block the calling thread until finished.
        //! Returns reference counted auto pointer, no need for manual memory management in upper level code.
        QueryResult PQuery(const char* sql, MySQLConnection* conn, ...)
        {
            if (!sql)
                return QueryResult(NULL);

            va_list ap;
            char szQuery[MAX_QUERY_LEN];
            va_start(ap, conn);
            vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
            va_end(ap);

            return Query(szQuery, conn);
        }

        //! Directly executes an SQL query in string format -with variable args- that will block the calling thread until finished.
        //! Returns reference counted auto pointer, no need for manual memory management in upper level code.
        QueryResult PQuery(const char* sql, ...)
        {
            if (!sql)
                return QueryResult(NULL);

            va_list ap;
            char szQuery[MAX_QUERY_LEN];
            va_start(ap, sql);
            vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
            va_end(ap);

            return Query(szQuery);
        }

        //! Directly executes an SQL query in prepared format that will block the calling thread until finished.
        //! Returns reference counted auto pointer, no need for manual memory management in upper level code.
        //! Statement must be prepared with CONNECTION_SYNCH flag.
        PreparedQueryResult Query(PreparedStatement* stmt)
        {
            T* t = GetFreeConnection();
            PreparedResultSet* ret = t->Query(stmt);
            t->Unlock();

            //! Delete proxy-class. Not needed anymore
            delete stmt;

            if (!ret || !ret->GetRowCount())

            {
                delete ret;
                return PreparedQueryResult(NULL);
            }
            return PreparedQueryResult(ret);
        }

        /**
            Asynchronous query (with resultset) methods.
        */

        //! Enqueues a query in string format that will set the value of the QueryResultFuture return object as soon as the query is executed.
        //! The return value is then processed in ProcessQueryCallback methods.
        QueryResultFuture AsyncQuery(const char* sql)
        {
            QueryResultFuture res;
            BasicStatementTask* task = new BasicStatementTask(sql, res);
            Enqueue(task);
            return res;
        }

        //! Enqueues a query in string format -with variable args- that will set the value of the QueryResultFuture return object as soon as the query is executed.
        //! The return value is then processed in ProcessQueryCallback methods.
        QueryResultFuture AsyncPQuery(const char* sql, ...)
        {
            va_list ap;
            char szQuery[MAX_QUERY_LEN];
            va_start(ap, sql);
            vsnprintf(szQuery, MAX_QUERY_LEN, sql, ap);
            va_end(ap);

            return AsyncQuery(szQuery);
        }

        //! Enqueues a query in prepared format that will set the value of the PreparedQueryResultFuture return object as soon as the query is executed.
        //! The return value is then processed in ProcessQueryCallback methods.
        //! Statement must be prepared with CONNECTION_ASYNC flag.
        PreparedQueryResultFuture AsyncQuery(PreparedStatement* stmt)
        {
            PreparedQueryResultFuture res;
            PreparedStatementTask* task = new PreparedStatementTask(stmt, res);
            Enqueue(task);
            return res;
        }

        //! Enqueues a query in prepared format that will set the value of the PreparedQueryResultFuture return object as soon as the query is executed.
        //! The return value is then processed in ProcessQueryCallback methods.
        //! Statement must be prepared with CONNECTION_ASYNC flag.
        PreparedQueryResultFuture AsyncQuery(PreparedStatement* stmt, QueryCallbackManagerPtr p_QueryCallbackMgr, std::function<void(PreparedQueryResult)> p_Callback)
        {
            PreparedQueryResultFuture res;
            PreparedStatementTask* task = new PreparedStatementTask(stmt, res);
            Enqueue(task);

            p_QueryCallbackMgr->AddPrepareStatementCallback(std::make_pair(p_Callback, res));

            return res;
        }

        //! Enqueues a vector of SQL operations (can be both adhoc and prepared) that will set the value of the QueryResultHolderFuture
        //! return object as soon as the query is executed.
        //! The return value is then processed in ProcessQueryCallback methods.
        //! Any prepared statements added to this holder need to be prepared with the CONNECTION_ASYNC flag.
        QueryResultHolderFuture DelayQueryHolder(SQLQueryHolder* holder)
        {
            QueryResultHolderFuture res;
            SQLQueryHolderTask* task = new SQLQueryHolderTask(holder, res);
            Enqueue(task);
            return res;
        }

        /**
            Transaction context methods.
        */

        //! Begins an automanaged transaction pointer that will automatically rollback if not commited. (Autocommit=0)
        SQLTransaction BeginTransaction()
        {
            return SQLTransaction(new Transaction);
        }

        //! Enqueues a collection of one-way SQL operations (can be both adhoc and prepared). The order in which these operations
        //! were appended to the transaction will be respected during execution.
        void CommitTransaction(SQLTransaction transaction, QueryCallbackManagerPtr p_QueryCallbackMgr = nullptr, MS::Utilities::CallBackPtr p_Callback = nullptr)
        {
            #ifdef TRINITY_DEBUG
            //! Only analyze transaction weaknesses in Debug mode.
            //! Ideally we catch the faults in Debug mode and then correct them,
            //! so there's no need to waste these CPU cycles in Release mode.
            switch (transaction->GetSize())
            {
                case 0:
                {
                    if (!p_Callback)
                    {
                        sLog->outDebug(LOG_FILTER_SQL_DRIVER, "Transaction contains 0 queries. Not executing.");
                        return;
                    }
                }
                case 1:
                    sLog->outDebug(LOG_FILTER_SQL_DRIVER, "Warning: Transaction only holds 1 query, consider removing Transaction context in code.");
                    break;
                default:
                    break;
            }
            #endif // TRINITY_DEBUG

            if (p_QueryCallbackMgr && p_Callback)
                p_QueryCallbackMgr->AddTransactionCallback(p_Callback);

            Enqueue(new TransactionTask(transaction, p_Callback));
        }

        bool DirectCommitTransaction(SQLTransaction& transaction)
        {
            T* con = GetFreeConnection();
            int errorCode = con->ExecuteTransaction(transaction);
            if (!errorCode)
            {
                con->Unlock();      // OK, operation succesful
                return true;
            }

            bool error = false;

            //! Handle MySQL Errno 1213 without extending deadlock to the core itself
            /// @todo More elegant way
            if (errorCode == 1213)
            {
                uint8 loopBreaker = 5;
                for (uint8 i = 0; i < loopBreaker; ++i)
                {
                    if (!con->ExecuteTransaction(transaction))
                    {
                        error = true;
                        break;
                    }
                }
            }

            //! Clean up now.
            transaction->Cleanup();

            con->Unlock();
            return error;
        }

        //! Method used to execute prepared statements in a diverse context.
        //! Will be wrapped in a transaction if valid object is present, otherwise executed standalone.
        void ExecuteOrAppend(SQLTransaction& trans, PreparedStatement* stmt)
        {
            if (!trans)
                Execute(stmt);
            else
                trans->Append(stmt);
        }

        //! Method used to execute ad-hoc statements in a diverse context.
        //! Will be wrapped in a transaction if valid object is present, otherwise executed standalone.
        void ExecuteOrAppend(SQLTransaction& trans, const char* sql)
        {
            if (!trans)
                Execute(sql);
            else
                trans->Append(sql);
        }

        /**
            Other
        */

        typedef typename T::Statements PreparedStatementIndex;

        //! Automanaged (internally) pointer to a prepared statement object for usage in upper level code.
        //! Pointer is deleted in this->Query(PreparedStatement*) or PreparedStatementTask::~PreparedStatementTask.
        //! This object is not tied to the prepared statement on the MySQL context yet until execution.
        PreparedStatement* GetPreparedStatement(PreparedStatementIndex index)
        {
            return new PreparedStatement(index);
        }

        //! Apply escape string'ing for current collation. (utf8)
        void EscapeString(std::string& str)
        {
            if (str.empty())
                return;

            char* buf = new char[str.size()*2+1];
            EscapeString(buf, str.c_str(), str.size());
            str = buf;
            delete[] buf;
        }

        //! Keeps all our MySQL connections alive, prevent the server from disconnecting us.
        void KeepAlive()
        {
            //! Ping synchronous connections
            for (uint8 i = 0; i < _connectionCount[IDX_SYNCH]; ++i)
            {
                T* t = _connections[IDX_SYNCH][i];
                if (t->LockIfReady())
                {
                    t->Ping();
                    t->Unlock();
                }
            }

            //! Assuming all worker threads are free, every worker thread will receive 1 ping operation request
            //! If one or more worker threads are busy, the ping operations will not be split evenly, but this doesn't matter
            //! as the sole purpose is to prevent connections from idling.
            for (size_t i = 0; i < _connections[IDX_ASYNC].size(); ++i)
                Enqueue(new PingOperation);
        }

    private:
        uint32 OpenConnections(InternalIndex type, uint8 numConnections)
        {
            _connections[type].resize(numConnections);
            for (uint8 i = 0; i < numConnections; ++i)
            {
                T* t = nullptr;

                if (type == IDX_ASYNC)
                    t = new T(_queue, *_connectionInfo);
                else if (type == IDX_SYNCH)
                    t = new T(*_connectionInfo);
                else
                    ASSERT(false);

                _connections[type][i] = t;
                ++_connectionCount[type];

                uint32 error = t->Open();

                // Failed to open a connection or invalid version, abort and cleanup
                if (error)
                {
                    while (_connectionCount[type] != 0)
                    {
                        T* t = _connections[type][i--];
                        delete t;
                        --_connectionCount[type];
                    }
                    return error;
                }
            }

            // Everything is fine
            return 0;
        }

        unsigned long EscapeString(char *to, const char *from, unsigned long length)
        {
            if (!to || !from || !length)
                return 0;

            return mysql_real_escape_string(_connections[IDX_SYNCH][0]->GetHandle(), to, from, length);
        }

        void Enqueue(SQLOperation* op)
        {
            _queue->enqueue(op);
        }

        //! Gets a free connection in the synchronous connection pool.
        //! Caller MUST call t->Unlock() after touching the MySQL context to prevent deadlocks.
        T* GetFreeConnection()
        {
            uint8 i = 0;
            size_t num_cons = _connectionCount[IDX_SYNCH];
            //! Block while there is connections, until a connection is free
            while (num_cons > 0)
            {
                T* t = _connections[IDX_SYNCH][++i % num_cons];
                //! Must be matched with t->Unlock() or you will get deadlocks
                if (t->LockIfReady())
                    return t;
            }

            //! This will be called when Celine Dion learns to sing
            return NULL;
        }

        char const* GetDatabaseName() const
        {
            return _connectionInfo->database.c_str();
        }

    private:
        //! Queue shared by async worker threads.
        moodycamel::ConcurrentQueue<SQLOperation*>* _queue;
        std::vector<std::vector<T*>> _connections;
        //! Counter of MySQL connections;
        uint32 _connectionCount[IDX_SIZE];
        std::unique_ptr<MySQLConnectionInfo> _connectionInfo;
        uint8 _async_threads, _synch_threads;
};

#endif
