/**
 * \file
 * \author TODO
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once


#include <algorithm> // std::copy
#include <numeric> // std::iota
#include "util/Random.hpp"
#include <utility> // std::swap
#include <vector>

#include "PeriodDimension.hpp"

namespace graphics::noise
{

    [[nodiscard]] constexpr int period_dimension_mask(PeriodDimension period_dimension) noexcept
    {
        return static_cast<int>(period_dimension) - 1;
    }

    template <class RandomAccessIter>
    void my_random_shuffle(RandomAccessIter first, RandomAccessIter last)
    {
        int target_index = 1;
        for (auto target = first + 1; target != last; ++target_index, ++target)
        {
            const int offset = util::random(target_index + 1);
            if (offset != target_index)
            {
                std::swap(*target, *(first + offset));
            }
        }
    }

    class PermutationHash
    {

    public:
        PermutationHash() = default;

         // TODO remove [[maybe_unused]] when implementing
        explicit PermutationHash([[maybe_unused]] PeriodDimension table_size)
        {
            // TODO implement this constructor
        }

        // TODO remove [[maybe_unused]] when implementing
        [[nodiscard]] int operator()([[maybe_unused]] int x) const noexcept
        {
            // TODO implement this function
        }

        // TODO remove [[maybe_unused]] when implementing
        [[nodiscard]] int operator()([[maybe_unused]] int x, [[maybe_unused]] int y) const noexcept
        {
            // TODO implement this function
        }

        // TODO remove [[maybe_unused]] when implementing
        [[nodiscard]] int operator()([[maybe_unused]] int x, [[maybe_unused]] int y, [[maybe_unused]] int z) const noexcept
        {
            // TODO implement this function
        }
    };

}