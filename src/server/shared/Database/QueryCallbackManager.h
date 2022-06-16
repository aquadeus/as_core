////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Common.h"
#include "QueryHolder.h"
#include "QueryResult.h"
#include "Callback.h"
#include "MSCallback.hpp"

struct QueryHolderCallback
{
    QueryHolderCallback(QueryResultHolderFuture p_QueryResultHolderFuture, std::function<void(SQLQueryHolder*)> p_Callback)
    {
        m_QueryResultHolderFuture = p_QueryResultHolderFuture;
        m_Callback = p_Callback;
    }

    QueryResultHolderFuture m_QueryResultHolderFuture;
    std::function<void(SQLQueryHolder*)>   m_Callback;
};

class QueryCallbackManager
{
    public:

        QueryCallbackManager();

        void AddTransactionCallback(std::shared_ptr<MS::Utilities::Callback> p_Callback);
        void AddPrepareStatementCallback(std::pair<std::function<void(PreparedQueryResult)>, PreparedQueryResultFuture> p_Callback);
        void AddQueryHolderCallback(QueryHolderCallback p_QueryHolderCallback);

        void Update();

    private:

        //////////////////////////////////////////////////////////////////////////
        /// New query holder callback system
        //////////////////////////////////////////////////////////////////////////
        using QueryHolderCallbacks = std::forward_list<QueryHolderCallback>;
        std::unique_ptr<QueryHolderCallbacks> m_QueryHolderCallbacks;
        std::unique_ptr<QueryHolderCallbacks> m_QueryHolderCallbacksBuffer;
        std::mutex m_QueryHolderCallbackLock;

        //////////////////////////////////////////////////////////////////////////
        /// New transaction query callback system
        //////////////////////////////////////////////////////////////////////////
        using TransactionCallbacks = std::forward_list<std::shared_ptr<MS::Utilities::Callback>>;
        std::unique_ptr<TransactionCallbacks> m_TransactionCallbacks;
        std::unique_ptr<TransactionCallbacks> m_TransactionCallbacksBuffer;
        std::mutex m_TransactionCallbackLock;

        //////////////////////////////////////////////////////////////////////////
        /// New prepare statement query callback system
        //////////////////////////////////////////////////////////////////////////
        using PrepareStatementCallback = std::pair<std::function<void(PreparedQueryResult)>, PreparedQueryResultFuture>;
        using PreparedStatementCallbacks = std::forward_list<PrepareStatementCallback>;
        std::unique_ptr<PreparedStatementCallbacks> m_PreparedStatementCallbacks;
        std::unique_ptr<PreparedStatementCallbacks> m_PreparedStatementCallbacksBuffer;
        std::mutex m_PreparedStatementCallbackLock;
};

using QueryCallbackManagerPtr = std::shared_ptr<QueryCallbackManager>;