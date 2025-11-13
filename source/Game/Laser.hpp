#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/Vec2.hpp"
#include <utility>
#include <vector>

constexpr CS200::RGBA COLOR_WARNING       = 0xFF000080;
constexpr CS200::RGBA COLOR_PARRY_WARNING = 0x00FFFF80;

namespace space
{
    class IRenderer2D;
}

class Star;

class Laser
{
public:
    Laser(Math::vec2 origin);

private:
};