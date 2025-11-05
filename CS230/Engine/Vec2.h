/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Vec2.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 18, 2025
*/

#pragma once
#include <limits>
#include <cmath>

namespace Math {
    struct vec2 {
        double x{ 0.0 };
        double y{ 0.0 };

        vec2() = default;
        vec2(double x, double y) : x(x), y(y) {}

        bool operator==(const vec2& v);
        bool operator!=(const vec2& v);

        vec2 operator+(const vec2& v);
        vec2& operator+=(const vec2& v);

        vec2 operator-(const vec2& v);
        vec2& operator-=(const vec2& v);

        vec2 operator*(double scale);
        vec2& operator*=(double scale);

        vec2 operator/(double divisor);
        vec2& operator/=(double divisor);

        vec2 operator-() const;

        vec2 Normalize() const;
    };

    vec2 operator*(double scale, const vec2& v);

    struct ivec2 {
        int x{ 0 };
        int y{ 0 };

        ivec2() = default;
        ivec2(int x, int y) : x(x), y(y) {};
        explicit operator vec2() {
            return vec2{ static_cast<double>(x),static_cast<double>(y) };
        }

        bool operator==(const ivec2& v);
        bool operator!=(const ivec2& v);

        ivec2 operator+(const ivec2& v);
        ivec2& operator+=(const ivec2& v);

        ivec2 operator-(const ivec2& v);
        ivec2& operator-=(const ivec2& v);

        ivec2 operator*(int scale);
        ivec2& operator*=(int scale);

        ivec2 operator/(int divisor);
        ivec2& operator/=(int divisor);

        vec2 operator*(double scale);
        vec2 operator/(double divisor);

        ivec2 operator-() const;
    };
}
