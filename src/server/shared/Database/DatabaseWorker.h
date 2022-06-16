////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _WORKERTHREAD_H
#define _WORKERTHREAD_H

#include <thread>
#include "ProducerConsumerQueue.h"
#include "ConcurrentQueue.h"

class MySQLConnection;
class SQLOperation;

class DatabaseWorker
{
    public:
        DatabaseWorker(moodycamel::ConcurrentQueue<SQLOperation*>* newQueue, MySQLConnection* connection);
        ~DatabaseWorker();

    private:
        moodycamel::ConcurrentQueue<SQLOperation*>* _queue;
        MySQLConnection* _connection;

        void WorkerThread();
        std::thread _workerThread;

        std::atomic_bool _cancelationToken;

        DatabaseWorker(DatabaseWorker const& right) = delete;
        DatabaseWorker& operator=(DatabaseWorker const& right) = delete;
};

#endif
