/**
 * \file
 * \author Junyeong Cho
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "JobSystem.hpp"
#include <iostream>

namespace util
{

#if defined(CAN_USE_THREADS)

    JobSystem::JobSystem() : isDone(false), jobsLeft(0)
    {
        unsigned num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0)
        {
            num_threads = 2;
        }

        for (unsigned i = 0; i < num_threads - 1; ++i)
        {
            workers.emplace_back(&JobSystem::WorkerThread, this);
        }
    }

    JobSystem::~JobSystem()
    {
        isDone = true;
        condition.notify_all();
        for (auto& worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void JobSystem::DoJob(Job job)
    {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            jobQueue.push(job);
        }
        jobsLeft.fetch_add(1);
        condition.notify_one();
    }

    void JobSystem::DoJobs(int how_many, ComputeAtIndex compute)
    {
        int num_threads      = static_cast<int>(std::max(1u, std::thread::hardware_concurrency()));
        int tasks_per_thread = how_many / num_threads;
        int remainder        = how_many % num_threads;

        for (int i = 0; i < num_threads; ++i)
        {
            int start_index = i * tasks_per_thread;
            int end_index   = start_index + tasks_per_thread - 1;
            if (i == num_threads - 1)
            {
                end_index += remainder;
            }

            DoJob(
                [=]
                {
                    for (int j = start_index; j <= end_index; ++j)
                    {
                        compute(j);
                    }
                });
        }
    }

    void JobSystem::WaitUntilDone()
    {
        while (jobsLeft.load() > 0)
        {
            Job job;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                if (condition.wait_for(lock, std::chrono::milliseconds(1), [&] { return !jobQueue.empty() || isDone; }))
                {
                    if (isDone && jobQueue.empty())
                    {
                        break;
                    }

                    job = jobQueue.front();
                    jobQueue.pop();
                }
            }
            if (job)
            {
                job();
                jobsLeft.fetch_sub(1);
            }
        }
    }

    void JobSystem::WorkerThread()
    {
        while (!isDone)
        {
            Job job;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] { return !jobQueue.empty() || isDone; });

                if (isDone && jobQueue.empty())
                {
                    return;
                }

                job = jobQueue.front();
                jobQueue.pop();
            }
            if (job)
            {
                job();
                jobsLeft.fetch_sub(1);
            }
        }
    }

    bool JobSystem::IsDone() const
    {
        return jobsLeft.load() == 0;
    }

#else

    void JobSystem::DoJob(Job job)
    {
        job();
    }

    void JobSystem::WaitUntilDone()
    {
    }

    void JobSystem::DoJobs(int how_many, JobSystem::ComputeAtIndex compute)
    {
        for (int i = 0; i < how_many; ++i)
        {
            compute(i);
        }
    }

    bool JobSystem::IsDone() const
    {
        return true;
    }

#endif
}
