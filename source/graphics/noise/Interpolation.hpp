/**
 * \file
 * \author TODO
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

namespace graphics::noise
{

    template <typename T>
    struct LinearValues
    {
        T Left{};
        T Right{};
    };

    // TODO remove [[maybe_unused]] when implementing
    template <typename T, typename U>
    constexpr auto linear_mix([[maybe_unused]] const LinearValues<T>& values, [[maybe_unused]] U s) noexcept
    {
        // TODO implement this function
    }

    template <typename T>
    struct BiLinearValues
    {
        LinearValues<T> Bottom{};
        LinearValues<T> Top{};
    };

    // TODO remove [[maybe_unused]] when implementing
    template <typename T, typename U>
    constexpr auto bilinear_mix([[maybe_unused]] const BiLinearValues<T>& values, [[maybe_unused]] U s, [[maybe_unused]] U t) noexcept
    {
        // TODO implement this function
    }

    template <typename T>
    struct TriLinearValues
    {
        BiLinearValues<T> Near{};
        BiLinearValues<T> Far{};
    };

    // TODO remove [[maybe_unused]] when implementing
    template <typename T, typename U>
    constexpr auto trilinear_mix([[maybe_unused]] const TriLinearValues<T>& values, [[maybe_unused]] U s, [[maybe_unused]] U t, [[maybe_unused]] U p) noexcept
    {
        // TODO implement this function
    }
}
