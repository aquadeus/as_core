////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "FunctionProcessor.hpp"

FunctionProcessor::FunctionProcessor()
{
    m_Time = 0;
}

FunctionProcessor::~FunctionProcessor()
{
    KillAllFunctions();
}

void FunctionProcessor::Update(uint32 p_Diff)
{
    /// Move from queue
    AddFunctionsFromQueue();

    /// Update time
    m_Time += p_Diff;

    std::lock_guard<std::recursive_mutex> l_Guard(m_FunctionsLock);

    /// Main event loop
    FunctionList::iterator l_Iter;
    while (((l_Iter = m_Functions.begin()) != m_Functions.end()) && l_Iter->first <= m_Time)
    {
        /// Get and remove event from queue
        l_Iter->second();
        m_Functions.erase(l_Iter);
    }
}

void FunctionProcessor::KillAllFunctions()
{
    std::lock_guard<std::recursive_mutex> l_FunctionsGuard(m_FunctionsLock);
    std::lock_guard<std::recursive_mutex> l_QueueGuard(m_QueueLock);

    m_Functions.clear();
    m_FunctionsQueue.clear();
}

void FunctionProcessor::AddFunction(std::function<void()>&& p_Function, uint64 p_EndTime)
{
    m_QueueLock.lock();
    m_FunctionsQueue.insert(std::pair<uint64, std::function<void()>>(p_EndTime, p_Function));
    m_QueueLock.unlock();
}

void FunctionProcessor::AddFunctionsFromQueue()
{
    m_QueueLock.lock();

    FunctionList::iterator l_Iter = m_FunctionsQueue.begin();

    m_FunctionsLock.lock();
    for(; l_Iter != m_FunctionsQueue.end(); ++l_Iter)
        m_Functions.insert(std::pair<uint64, std::function<void()>>(l_Iter->first, l_Iter->second));
    m_FunctionsLock.unlock();

    m_FunctionsQueue.clear();
    m_QueueLock.unlock();
}

uint64 FunctionProcessor::CalculateTime(uint64 p_Offset) const
{
    return (m_Time + p_Offset);
}
