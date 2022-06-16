#include "ThreadPoolMap.hpp"

ThreadPoolMap::ThreadPoolMap()
    : requestCount_(0)
{ }

void ThreadPoolMap::start(std::size_t numThreads)
{
    threads_.reserve(numThreads);
    for (std::size_t i = 0; i < numThreads; ++i)
        threads_.emplace_back(&ThreadPoolMap::threadFunc, this);
}

void ThreadPoolMap::stop()
{
    if (!queue_.frozen()) {
        queue_.freeze();
        for (auto &t : threads_)
            t.join();
    }
}

void ThreadPoolMap::wait()
{
    GuardType guard(lock_);
    waitCond_.wait(guard, [this] { return requestCount_ == 0; });
}

void ThreadPoolMap::threadFunc()
{
    FunctorType f;
    while (queue_.pop(f)) {
        f();
        GuardType g(lock_);
        if (--requestCount_ == 0)
            waitCond_.notify_all();
    }
}

static thread_local ThreadPoolMap tl_ThreadPoolMap;

ThreadPoolMap& ThreadPoolMap::Get()
{
    return tl_ThreadPoolMap;
}
