////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef OUTDOOR_PVE_H_
#define OUTDOOR_PVE_H_

#include "SharedDefines.h"
#include "ZoneScript.h"

// base class for specific outdoor pve handlers
class OutdoorPvEScript : public ZoneScript
{
    public:
        explicit OutdoorPvEScript(Map* p_Map) :
            m_Map(p_Map) {}

    public:
        virtual void Initialize() {}

        virtual void Update(uint32 const p_Diff) {}

        virtual void OnScenarioCreate(uint32 p_ScenarioGuid) {}
        virtual void OnScenarioNextStep(uint32 p_NewStep) {}
        virtual void OnScenarioComplete() {}

        virtual void OnPlayerEnter(Player* p_Player) {}

        Map* GetMap()
        {
            return m_Map;
        }

        /// Add timed delayed operation
        /// @p_Timeout  : Delay time
        /// @p_Function : Callback function
        void AddTimedDelayedOperation(uint32 p_Timeout, std::function<void()> && p_Function)
        {
            m_EmptyWarned = false;
            m_TimedDelayedOperations.push_back(std::pair<uint32, std::function<void()>>(p_Timeout, p_Function));
        }

        void UpdateOperations(uint32 const p_Diff)
        {
            std::vector<std::function<void()>> l_OperationsReady;

            for (auto l_Iter = m_TimedDelayedOperations.begin(); l_Iter != m_TimedDelayedOperations.end();)
            {
                auto& l_Pair = *l_Iter;

                l_Pair.first -= p_Diff;

                if (l_Pair.first < 0)
                {
                    l_OperationsReady.push_back(l_Pair.second);

                    l_Iter = m_TimedDelayedOperations.erase(l_Iter);
                }
                else
                    ++l_Iter;
            }

            for (auto l_Operation : l_OperationsReady)
                l_Operation();

            if (m_TimedDelayedOperations.empty() && !m_EmptyWarned)
            {
                m_EmptyWarned = true;
            }
        }

        std::list<std::pair<int32, std::function<void()>>>      m_TimedDelayedOperations;   ///< Delayed operations
        bool                                                    m_EmptyWarned;              ///< Warning when there are no more delayed operations

    private:
        Map* m_Map;
};

#endif /*OUTDOOR_PVE_H_*/
