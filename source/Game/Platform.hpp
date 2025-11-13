#pragma once
#include "Engine/Vec2.hpp"

struct Platform
{
    Math::vec2 start{};
    Math::vec2 end{};
    double     thickness   = 20.0;
    bool       canDropDown = false;
};