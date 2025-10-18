/**
 * \file
 * \author Rudy Castan
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <array>
#include <cstdint>

namespace CS200
{
    using RGBA           = uint32_t;
    constexpr RGBA WHITE = 0xffffffff;
    constexpr RGBA BLACK = 0x000000ff;
    constexpr RGBA CLEAR = 0x00000000;

    constexpr std::array<float, 4> unpack_color(RGBA rgba) noexcept
    {
        constexpr float scale = 1.0f / 255.0f;
        return { static_cast<float>((rgba & 0xff000000) >> 24) * scale, static_cast<float>((rgba & 0x00ff0000) >> 16) * scale, static_cast<float>((rgba & 0x0000ff00) >> 8) * scale,
                 static_cast<float>((rgba & 0x000000ff) >> 0) * scale };
    }
}
