#pragma once

#include "Engine/Vec2.hpp"
#include "CS200/RGBA.hpp"
#include <vector>
#include <utility>

// DemoReflection의 색상 상수
constexpr CS200::RGBA COLOR_RED   = 0xFF0000FF;
constexpr CS200::RGBA COLOR_GREEN = 0x00FF00FF;

namespace CS200
{
    class IRenderer2D;
}

namespace Math
{
    class TransformationMatrix;
}

/**
 * \brief DemoReflection의 타겟 로직을 관리하는 클래스입니다.
 *
 * 타겟의 상태, 그리기, 레이저와의 충돌 검사를 담당합니다.
 */
class Target
{
public:
    Target(Math::vec2 pos);

    void Update(const std::vector<std::pair<Math::vec2, Math::vec2>>& parriedLaserPath);
    void Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const;
    void Reset();

private:
    bool CheckCollision(const std::vector<std::pair<Math::vec2, Math::vec2>>& laserPath) const;

    Math::vec2  position;
    CS200::RGBA color             = COLOR_RED;
    double      radius            = 25.0;
    bool        hitByParriedLaser = false;
};