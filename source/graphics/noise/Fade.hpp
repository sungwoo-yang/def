/**
 * \file
 * \author TODO
 * \date 2024 Spring
 * \par CS250 Computer Graphics II
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <cmath>

namespace graphics::noise
{
    enum class SmoothMethod
    {
        Linear,
        Cosine,
        Smoothstep,
        Quintic
    };

    // TODO remove [[maybe_unused]] when implementing
    constexpr auto fade([[maybe_unused]] float x, [[maybe_unused]] SmoothMethod smoothing) noexcept
    {
        // TODO implement this function
    }

    // TODO remove [[maybe_unused]] when implementing
    constexpr auto fade([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] SmoothMethod smoothing) noexcept
    {
        // TODO implement this function
    }

    // TODO remove [[maybe_unused]] when implementing
    constexpr auto fade([[maybe_unused]] float x, [[maybe_unused]] float y, [[maybe_unused]] float z, [[maybe_unused]] SmoothMethod smoothing) noexcept
    {
        // TODO implement this function
    }
}