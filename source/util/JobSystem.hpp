/**
 * \file
 * \author TODO
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "window/Environment.hpp"


#if defined(CAN_USE_THREADS)

// TODO - replace with your own JobSystem implementation from CS180
#    include <functional>

namespace util
{

    class JobSystem
    {
    public:
        using Job = std::function<void(void)>;
        void DoJob(Job job);
        void WaitUntilDone();
        bool IsDone() const;

        using ComputeAtIndex = std::function<void(int)>;
        void DoJobs(int how_many, ComputeAtIndex compute);
    };
}

#else
#    include <functional>

namespace util
{

    class JobSystem
    {
    public:
        using Job = std::function<void(void)>;
        void DoJob(Job job);
        void WaitUntilDone();
        bool IsDone() const;

        using ComputeAtIndex = std::function<void(int)>;
        void DoJobs(int how_many, ComputeAtIndex compute);
    };
}

#endif
