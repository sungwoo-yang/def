#include "Star.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include <cmath>
#include <limits>

namespace
{
    bool LineCircleIntersection(Math::vec2 p1, Math::vec2 p2, Math::vec2 center, double radius, Math::vec2& intersection)
    {
        Math::vec2 d = p2 - p1;
        Math::vec2 f = p1 - center;
        double     a = dot(d, d);
        double     b = 2.0 * dot(f, d);
        double     c = dot(f, f) - radius * radius;

        double discriminant = b * b - 4.0 * a * c;
        if (discriminant < 0)
        {
            return false;
        }
        else
        {
            discriminant       = std::sqrt(discriminant);
            double t1          = (-b - discriminant) / (2.0 * a);
            double t2          = (-b + discriminant) / (2.0 * a);
            bool   intersected = false;

            if (t1 >= -std::numeric_limits<double>::epsilon() && t1 <= 1.0 + std::numeric_limits<double>::epsilon())
            {
                intersection = p1 + d * t1;
                intersected  = true;
            }
            if (t2 >= -std::numeric_limits<double>::epsilon() && t2 <= 1.0 + std::numeric_limits<double>::epsilon())
            {
                if (!intersected)
                {
                    intersection = p1 + d * t2;
                }
                intersected = true;
            }
            return intersected;
        }
    }
} // 익명 네임스페이스 끝

Target::Target(Math::vec2 pos) : position(pos), color(COLOR_RED), radius(25.0), hitByParriedLaser(false)
{
}

void Target::Update(const std::vector<std::pair<Math::vec2, Math::vec2>>& parriedLaserPath)
{
    if (!hitByParriedLaser && CheckCollision(parriedLaserPath))
    {
        color             = COLOR_GREEN;
        hitByParriedLaser = true;
        Engine::GetLogger().LogEvent("Target hit by parried laser!");
    }
}

void Target::Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const
{
    // 카메라 매트릭스 적용
    renderer.DrawCircle(camera_matrix * Math::TranslationMatrix(position) * Math::ScaleMatrix(radius), color);
}

void Target::Reset()
{
    color             = COLOR_RED;
    hitByParriedLaser = false;
}

bool Target::CheckCollision(const std::vector<std::pair<Math::vec2, Math::vec2>>& laserPath) const
{
    Math::vec2 intersectionPoint;
    for (const auto& segment : laserPath)
    {
        if (LineCircleIntersection(segment.first, segment.second, position, radius, intersectionPoint))
        {
            return true;
        }
    }
    return false;
}