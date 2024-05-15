/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "environment/Environment.hpp"
#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace util
{

#if defined(CAN_USE_THREADS)

    class JobSystem
    {
    public:
        using Job            = std::function<void(void)>;
        using ComputeAtIndex = std::function<void(int)>;

        JobSystem();
        ~JobSystem();

        void DoJob(Job job);
        void DoJobs(int how_many, ComputeAtIndex compute);
        void WaitUntilDone();
        bool IsDone() const;

        JobSystem(const JobSystem&)            = delete;
        JobSystem(JobSystem&&)                 = delete;
        JobSystem& operator=(const JobSystem&) = delete;
        JobSystem& operator=(JobSystem&&)      = delete;

    private:
        void WorkerThread();

        std::vector<std::thread> workers;
        std::queue<Job>          jobQueue;

        std::mutex              queueMutex;
        std::condition_variable condition;

        std::atomic<bool> isDone;
        std::atomic<int>  jobsLeft;
    };

#else

    class JobSystem
    {
    public:
        using Job            = std::function<void(void)>;
        using ComputeAtIndex = std::function<void(int)>;

        void DoJob(Job job);
        void DoJobs(int how_many, ComputeAtIndex compute);
        void WaitUntilDone();
        bool IsDone() const;
    };

#endif
}
