////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2018 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include "QueryCallbackManager.h"

QueryCallbackManager::QueryCallbackManager()
{
    m_QueryHolderCallbacks             = std::unique_ptr<QueryHolderCallbacks>(new QueryHolderCallbacks());
    m_QueryHolderCallbacksBuffer       = std::unique_ptr<QueryHolderCallbacks>(new QueryHolderCallbacks());
    m_TransactionCallbacks             = std::unique_ptr<TransactionCallbacks>(new TransactionCallbacks());
    m_TransactionCallbacksBuffer       = std::unique_ptr<TransactionCallbacks>(new TransactionCallbacks());
    m_PreparedStatementCallbacks       = std::unique_ptr<PreparedStatementCallbacks>(new PreparedStatementCallbacks());
    m_PreparedStatementCallbacksBuffer = std::unique_ptr<PreparedStatementCallbacks>(new PreparedStatementCallbacks());
}

void QueryCallbackManager::AddTransactionCallback(std::shared_ptr<MS::Utilities::Callback> p_Callback)
{
    m_TransactionCallbackLock.lock();
    m_TransactionCallbacksBuffer->push_front(p_Callback);
    m_TransactionCallbackLock.unlock();
}

void QueryCallbackManager::AddPrepareStatementCallback(std::pair<std::function<void(PreparedQueryResult)>, PreparedQueryResultFuture> p_Callback)
{
    m_PreparedStatementCallbackLock.lock();
    m_PreparedStatementCallbacksBuffer->push_front(p_Callback);
    m_PreparedStatementCallbackLock.unlock();
}

void QueryCallbackManager::AddQueryHolderCallback(QueryHolderCallback p_QueryHolderCallback)
{
    m_QueryHolderCallbackLock.lock();
    m_QueryHolderCallbacksBuffer->push_front(p_QueryHolderCallback);
    m_QueryHolderCallbackLock.unlock();
}

void QueryCallbackManager::Update()
{
    /// - Update querys holders callbacks
    if (!m_QueryHolderCallbacks->empty())
    {
        m_QueryHolderCallbacks->remove_if([](QueryHolderCallback const& p_Callback) -> bool
        {
            if (p_Callback.m_QueryResultHolderFuture.ready())
            {
                SQLQueryHolder* l_QueryHolder;
                p_Callback.m_QueryResultHolderFuture.get(l_QueryHolder);

                p_Callback.m_Callback(l_QueryHolder);
                return true;
            }

            return false;
        });
    }

    /// - Add querys holders callbacks in buffer queue to real queue
    while (!m_QueryHolderCallbacksBuffer->empty())
    {
        m_QueryHolderCallbacks->push_front(m_QueryHolderCallbacksBuffer->front());
        m_QueryHolderCallbacksBuffer->pop_front();
    }

    /// - Update transactions callbacks
    if (!m_TransactionCallbacks->empty())
    {
        m_TransactionCallbacks->remove_if([](MS::Utilities::CallBackPtr const& l_Callback) -> bool
        {
            if (l_Callback->m_State == MS::Utilities::CallBackState::Waiting)
                return false;

            l_Callback->m_CallBack(l_Callback->m_State == MS::Utilities::CallBackState::Success);
            return true;
        });
    }

    /// - Add transactions callbacks in buffer queue to real queue
    while (!m_TransactionCallbacksBuffer->empty())
    {
        m_TransactionCallbacks->push_front(m_TransactionCallbacksBuffer->front());
        m_TransactionCallbacksBuffer->pop_front();
    }

    /// - Update prepared statements callbacks
    if (!m_PreparedStatementCallbacks->empty())
    {
        m_PreparedStatementCallbacks->remove_if([](PrepareStatementCallback const& p_Callback) -> bool
        {
            /// If the query result is avaiable ...
            if (p_Callback.second.ready())
            {
                /// Then get it
                PreparedQueryResult l_Result;
                p_Callback.second.get(l_Result);

                /// Give the result to the callback, and execute it
                p_Callback.first(l_Result);

                /// Delete the callback from the forward list
                return true;
            }

            /// We havn't the query result yet, we keep the callback and wait for the result!
            return false;
        });
    }

    /// - Add prepared statements in buffer queue to real queue
    while (!m_PreparedStatementCallbacksBuffer->empty())
    {
        m_PreparedStatementCallbacks->push_front(m_PreparedStatementCallbacksBuffer->front());
        m_PreparedStatementCallbacksBuffer->pop_front();
    }
}