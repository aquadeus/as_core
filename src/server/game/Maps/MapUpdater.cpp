////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#include <condition_variable>

#include "Common.h"
#include "MapUpdater.h"
#include "Map.h"

/// Constructor
MapUpdaterTask::MapUpdaterTask(MapUpdater* p_Updater)
    : m_updater(p_Updater)
{

}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

/// Notify that the task is done
void MapUpdaterTask::UpdateFinished()
{
    if (m_updater != nullptr)
        m_updater->update_finished();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class MapUpdateRequest : MapUpdaterTask
{
    private:
        Map& m_Map;
        uint32 m_Diff;

    public:
        MapUpdateRequest(Map& m, MapUpdater& u, uint32 p_Diff)
            : MapUpdaterTask(&u), m_Map(m), m_Diff(p_Diff)
        {
        }

        void call() override
        {
            m_Map.SetUpdating(true);
            m_Map.Update(m_Map.GetId(), m_Diff);
            m_Map.SetUpdating(false);
            UpdateFinished();
        }

        uint32 GetAverageDelay() const
        {
            return m_Map.GetAverageDelay();
        }
};

void MapUpdater::activate(size_t num_threads)
{
    for (size_t i = 0; i < num_threads; ++i)
        _workerThreads.push_back(std::thread(&MapUpdater::WorkerThread, this));
}

void MapUpdater::deactivate()
{
    _cancelationToken = true;

    wait();

    _queue.Cancel();

    for (auto& thread : _workerThreads)
    {
        thread.join();
    }
}

void MapUpdater::wait()
{
    std::unique_lock<std::mutex> lock(_lock);

    while (pending_requests > 0)
        _condition.wait(lock);

    lock.unlock();
}

void MapUpdater::schedule_update(Map& p_Map, uint32 p_Diff)
{
    std::lock_guard<std::mutex> lock(_lock);

    ++pending_requests;

    _queue.Push((MapUpdaterTask*)new MapUpdateRequest(p_Map, *this, p_Diff));
}

void MapUpdater::schedule_specific(MapUpdaterTask* p_Request)
{
    std::lock_guard<std::mutex> lock(_lock);

    ++pending_requests;

    _queue.Push(p_Request);
}

bool MapUpdater::activated()
{
    return _workerThreads.size() > 0;
}

void MapUpdater::update_finished()
{
    std::lock_guard<std::mutex> lock(_lock);

    --pending_requests;

    _condition.notify_all();
}

void MapUpdater::WorkerThread()
{
    ThreadPoolMap::Get().start(sWorld->getIntConfig(CONFIG_MAP_THREAD_POOL_COUNT));

    while (1)
    {
        MapUpdaterTask* request = nullptr;

        _queue.WaitAndPop(request);

        if (_cancelationToken)
        {
            ThreadPoolMap::Get().stop();
            return;
        }

        request->call();

        delete request;
    }
}