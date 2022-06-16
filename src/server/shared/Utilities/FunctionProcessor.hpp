////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2017 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef FUNCTION_PROCESSOR_HPP
# define FUNCTION_PROCESSOR_HPP

# include "Define.h"
# include "Common.h"

# include <map>

typedef std::multimap<uint64, std::function<void()>> FunctionList;

class FunctionProcessor
{
    public:
        FunctionProcessor();
        ~FunctionProcessor();

        void Update(uint32 p_Diff);
        void KillAllFunctions();
        void AddFunction(std::function<void()>&& p_Function, uint64 p_EndTime);
        void AddFunctionsFromQueue();
        uint64 CalculateTime(uint64 p_Offset) const;
        bool Empty() const { return m_Functions.empty(); }
        uint32 Size() const { return m_Functions.size(); }
        uint32 SizeQueue() const { return m_FunctionsQueue.size(); }

    protected:
        std::atomic<uint64> m_Time;
        FunctionList m_Functions;
        FunctionList m_FunctionsQueue;
        std::recursive_mutex m_QueueLock;
        std::recursive_mutex m_FunctionsLock;
};

#endif ///< FUNCTION_PROCESSOR_HPP
