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
    struct [[nodiscard]] NoiseCoordinate
    {
        int   base        = 0;
        int   next        = 0;
        float interpolant = 0.0f;
    };

    // TODO remove [[maybe_unused]] when implementing
    constexpr NoiseCoordinate make_noise_coord([[maybe_unused]] float input) noexcept
    {
        // TODO implement this function
    }

}