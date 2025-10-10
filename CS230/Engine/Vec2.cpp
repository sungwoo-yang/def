/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Vec2.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 18, 2025
*/

#include "Vec2.h"

//vec2

bool Math::vec2::operator==(const vec2& v) {
    return x == v.x && y == v.y;
}

bool Math::vec2::operator!=(const vec2& v) {
    return x != v.x || y != v.y;
}

Math::vec2 Math::vec2::operator+(const vec2& v) {
    return { x + v.x, y + v.y };
}

Math::vec2& Math::vec2::operator+=(const vec2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Math::vec2 Math::vec2::operator-(const vec2& v) {
    return { x - v.x,y - v.y };
}

Math::vec2& Math::vec2::operator-=(const vec2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Math::vec2 Math::vec2::operator*(double scale) {
    return { x * scale, y * scale };
}

Math::vec2& Math::vec2::operator*=(double scale) {
    x *= scale;
    y *= scale;
    return *this;
}

Math::vec2 Math::vec2::operator/(double divisor) {
    return { x / divisor, y / divisor };
}

Math::vec2& Math::vec2::operator/=(double divisor) {
    x /= divisor;
    y /= divisor;
    return *this;
}

Math::vec2 Math::operator*(double scale, const vec2& v) {
    return { scale * v.x, scale * v.y };
}

Math::vec2 Math::vec2::operator-() const {
    return { -x, -y };
}

Math::vec2 Math::vec2::Normalize() const {
    double len = std::sqrt(x * x + y * y);
    if (len == 0.0)
        return { 0.0, 0.0 };
    return { x / len, y / len };
}

//ivec2
bool Math::ivec2::operator==(const ivec2& v) {
    return x == v.x && y == v.y;
}

bool Math::ivec2::operator!=(const ivec2& v) {
    return x != v.x || y != v.y;
}

Math::ivec2 Math::ivec2::operator+(const ivec2& v) {
    return { x + v.x, y + v.y };
}

Math::ivec2& Math::ivec2::operator+=(const ivec2& v) {
    x += v.x;
    y += v.y;
    return *this;
}

Math::ivec2 Math::ivec2::operator-(const ivec2& v) {
    return { x - v.x,y - v.y };
}

Math::ivec2& Math::ivec2::operator-=(const ivec2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Math::ivec2 Math::ivec2::operator*(int scale) {
    return { x * scale, y + scale };
}

Math::ivec2& Math::ivec2::operator*=(int scale) {
    x *= scale;
    y *= scale;
    return *this;
}

Math::ivec2 Math::ivec2::operator/(int divisor) {
    return { x / divisor, y / divisor };
}

Math::ivec2& Math::ivec2::operator/=(int divisor) {
    x /= divisor;
    y /= divisor;
    return *this;
}

Math::vec2 Math::ivec2::operator*(double scale) {
    return { x * scale, y * scale };
}
Math::vec2 Math::ivec2::operator/(double divisor) {
    return { x / divisor, y / divisor };
}

Math::ivec2 Math::ivec2::operator-() const {
    return { -x, -y };
}