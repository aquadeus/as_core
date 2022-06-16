////////////////////////////////////////////////////////////////////////////////
///
/// MILLENIUM-STUDIO
/// Copyright 2018 Millenium-studio SARL
/// All Rights Reserved.
///
////////////////////////////////////////////////////////////////////////////////

#include "GarbageCollector.h"

void GarbageCollector::Runnable()
{
    while (!World::IsStopped())
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
        sGarbageCollector->Collect();
    }
}

GarbageCollector::GarbageCollector()
{
    m_GarbageCollectorRunnable = new std::thread(&GarbageCollector::Runnable, this);
}

GarbageCollector* GarbageCollector::Get()
{
    return s_GarbageCollector;
}
