/**
 * \file
 * \author Rudy Castan
 * \author Sungwoo Yang
 * \date 2025 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include "PeriodDimension.hpp"
#include "util/Random.hpp"

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

namespace graphics::noise
{
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

        explicit PermutationHash(PeriodDimension table_size)
            : mask(period_dimension_mask(table_size))
        {
            const int size = static_cast<int>(table_size);

            permutation.resize(static_cast<size_t>(size));
            std::iota(permutation.begin(), permutation.end(), 0);
            my_random_shuffle(permutation.begin(), permutation.end());
        }

        [[nodiscard]] int operator()(int x) const noexcept
        {
            return permutation[static_cast<size_t>(x & mask)];
        }

        [[nodiscard]] int operator()(int x, int y) const noexcept
        {
            return permutation[static_cast<size_t>((permutation[static_cast<size_t>(x & mask)] + y) & mask)];
        }

        [[nodiscard]] int operator()(int x, int y, int z) const noexcept
        {
            const int hashed_x  = permutation[static_cast<size_t>(x & mask)];
            const int hashed_xy = permutation[static_cast<size_t>((hashed_x + y) & mask)];

            return permutation[static_cast<size_t>((hashed_xy + z) & mask)];
        }

    private:
        int              mask = 0;
        std::vector<int> permutation;
    };
}