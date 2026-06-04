/**
 * \file
 * \author TODO
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */

#include "JobSystem.hpp"

using namespace std;

namespace util
{
#if defined(CAN_USE_THREADS)

    // TODO - replace with your own JobSystem implementation from CS180

    void JobSystem::DoJob(JobSystem::Job job)
    {
        job();
    }

    void JobSystem::WaitUntilDone()
    {
    }

    void JobSystem::DoJobs(int how_many, JobSystem::ComputeAtIndex compute)
    {
        for (int i = 0; i < how_many; ++i)
            compute(i);
    }

    bool JobSystem::IsDone() const
    {
        return true;
    }

#else

    void JobSystem::DoJob(JobSystem::Job job)
    {
        job();
    }

    void JobSystem::WaitUntilDone()
    {
    }

    void JobSystem::DoJobs(int how_many, JobSystem::ComputeAtIndex compute)
    {
        for (int i = 0; i < how_many; ++i)
            compute(i);
    }

    bool JobSystem::IsDone() const
    {
        return true;
    }
#endif
}
