////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#ifndef _GARBAGECOLLECTOR_H
#define _GARBAGECOLLECTOR_H

#include "World.h"
#include "boost/any.hpp"

#define Garbage(T) std::queue<std::pair<T*, uint32>>

class GarbageCollector
{
    public:
        GarbageCollector();

        template <class GARBAGE>
        void Add(GARBAGE* p_Garbage)
        {
            std::lock_guard<std::recursive_mutex> l_Lock(m_GarbageCollectorLock);

            uint32 l_Index = 0;
            for (boost::any& l_Info : m_ContainerInformations)
            {
                if (l_Info.type() == boost::any(p_Garbage).type())
                    break;

                l_Index++;
            }

            if (l_Index >= m_ContainerInformations.size())
            {
                m_ContainerInformations.push_back(boost::any(p_Garbage));
                m_Containers.push_back(new Garbage(GARBAGE));
                AddCollector(p_Garbage);
                return;
            }

            ((Garbage(GARBAGE)*)m_Containers[l_Index])->push({ p_Garbage, sWorld->GetGameTime() + sWorld->getIntConfig(CONFIG_GC_TIMER) });
        }

        void Collect()
        {
            std::lock_guard<std::recursive_mutex> l_Lock(m_GarbageCollectorLock);

            for (auto& l_Collector : m_Collectors)
                l_Collector();
        }

        void Runnable();

        static GarbageCollector* Get();

    private:

        template<class GARBAGE>
        void AddCollector(GARBAGE* p_Type)
        {
            m_Collectors.push_back([p_Type, this]() -> void
            {
                uint32 l_Index = 0;
                for (boost::any& l_Info : m_ContainerInformations)
                {
                    if (l_Info.type() == boost::any(p_Type).type())
                        break;

                    l_Index++;
                }

                ASSERT(l_Index < m_ContainerInformations.size());

                uint32 p_Time = sWorld->GetGameTime();
                auto l_Queue = ((Garbage(GARBAGE)*)m_Containers[l_Index]);
                while (!l_Queue->empty())
                {
                    auto& l_Element = l_Queue->front();
                    if (l_Element.second > p_Time)
                        break;

                    delete l_Element.first;
                    l_Queue->pop();
                }
            });
        }

        mutable std::recursive_mutex m_GarbageCollectorLock;

        std::thread* m_GarbageCollectorRunnable;
        std::vector<std::function<void()>> m_Collectors;

        std::vector<boost::any> m_ContainerInformations;
        std::vector<void*> m_Containers;
};

static GarbageCollector* s_GarbageCollector = new GarbageCollector();
#define sGarbageCollector GarbageCollector::Get()

#endif