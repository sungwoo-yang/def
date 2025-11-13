#include "Game/Laser.hpp"
#include "Game/Target.hpp" // 전방 선언의 구현 포함
#include "Engine/Physics/Reflection.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Engine/Logger.hpp"
#include "CS200/IRenderer2D.hpp"
#include <cmath>
#include <limits>

Laser::Laser(Math::vec2 origin) : laserOrigin(origin)
{
    laserColor = COLOR_RED;
    isLaserOn = false;
    wasLaserOn = false;
    showingWarningLaser = false;
    laserTimer = 0.0;
}

bool Laser::Update(double dt, bool playerIsParrying, const std::vector<std::pair<Math::vec2, Math::vec2>>& reflectionSegments)
{
    laserTimer += dt;
    double cycleTime = fmod(laserTimer, laserCycleTime);

    wasLaserOn = isLaserOn;
    showingWarningLaser = !isLaserOn && (cycleTime >= laserCycleTime - warningLaserLeadTime);
    isLaserOn           = (cycleTime < laserOnDuration);

    bool parryWindowActive = false;
    
    if (showingWarningLaser)
    {
        double timeIntoWarning = cycleTime - (laserCycleTime - warningLaserLeadTime);

        if (timeIntoWarning >= parryWindowStartTimeOffset)
        {
            parryWindowActive = true;
            warningLaserColor = COLOR_PARRY_WARNING;
        }
        else
        {
            warningLaserColor = COLOR_WARNING;
        }

        // 경고 레이저 경로 계산 (화면 중앙 조준)
        const auto windowSize = CS230::Engine::GetWindow().GetSize();
        Math::vec2 center     = { windowSize.x / 2.0, windowSize.y / 2.0 };
        Math::vec2 initialDir = (center - laserOrigin).Normalize();
        if (initialDir.Length() < std::numeric_limits<double>::epsilon())
        {
            initialDir = { 1.0, -1.0 }; // 기본값
            initialDir = initialDir.Normalize();
        }
        
        // 경고 레이저는 플레이어 쉴드가 아닌 반사 세그먼트(예: 맵의 거울)에만 반응
        warningLaserPath = Physics::CalculateLaserPath(laserOrigin, initialDir, reflectionSegments, 1); // 1회 반사
        laserPath.clear();
    }
    else
    {
        warningLaserColor = COLOR_WARNING;
        warningLaserPath.clear();
    }

    // 레이저가 켜지는 순간이 아니면 패링 상태 리셋
    if (!isLaserOn)
    {
        laserColor = COLOR_RED;
    }
    
    return parryWindowActive;
}

bool Laser::CheckParrySuccess(bool playerIsParrying)
{
    // 레이저가 "방금" 켜졌는지 확인
    if (isLaserOn && !wasLaserOn)
    {
        if (playerIsParrying)
        {
            laserColor = COLOR_YELLOW;
            CS230::Engine::GetLogger().LogEvent("Laser turned YELLOW (Parry)");
            return true;
        }
        else
        {
            laserColor = COLOR_RED;
            CS230::Engine::GetLogger().LogEvent("Laser turned RED (Parry Failed)");
            return false;
        }
    }
    return false; // 레이저가 켜지는 순간이 아님
}

void Laser::CalculateLaserPath(const std::vector<std::pair<Math::vec2, Math::vec2>>& reflectionSegments)
{
    if (!isLaserOn)
    {
        laserPath.clear();
        return;
    }

    const auto windowSize = CS230::Engine::GetWindow().GetSize();
    Math::vec2 center     = { windowSize.x / 2.0, windowSize.y / 2.0 };
    Math::vec2 initialDir = (center - laserOrigin).Normalize();
    if (initialDir.Length() < std::numeric_limits<double>::epsilon())
    {
        initialDir = { 1.0, -1.0 };
        initialDir = initialDir.Normalize();
    }

    if (laserColor == COLOR_YELLOW) // 패링 성공
    {
        laserPath = Physics::CalculateLaserPath(laserOrigin, initialDir, reflectionSegments, 5);
    }
    else // 패링 아님 (빨간 레이저)
    {
        // 빈 세그먼트를 전달하여 반사 없음
        laserPath = Physics::CalculateLaserPath(laserOrigin, initialDir, {}, 5);
    }
}


bool Laser::CheckShieldCollision(const std::vector<std::pair<Math::vec2, Math::vec2>>& shieldSegments) const
{
    if (!isLaserOn || laserColor != COLOR_RED)
    {
        return false;
    }

    // 레이저의 첫 번째 세그먼트(반사되기 전)만 쉴드와 충돌할 수 있음
    if (laserPath.empty())
    {
        return false;
    }

    const auto& laserSegment = laserPath[0];
    
    Math::vec2 intersectionPoint;
    double     t;

    for(const auto& shieldSegment : shieldSegments)
    {
        // Physics::RaySegmentIntersection은 레이저 '광선'과 '선분'의 교차를 확인합니다.
        // DemoReflection에서는 레이저의 첫 번째 경로가 (origin -> center) 방향의 광선이었습니다.
        // 여기서는 이미 계산된 laserPath의 첫 번째 선분을 사용합니다.
        // 따라서 광선-선분 검사가 아닌 선분-선분 검사가 필요하지만,
        // 여기서는 DemoReflection의 로직을 따라 광선-선분 검사를 사용합니다.
        
        const auto windowSize = CS230::Engine::GetWindow().GetSize();
        Math::vec2 center     = { windowSize.x / 2.0, windowSize.y / 2.0 };
        Math::vec2 initialDir = (center - laserOrigin).Normalize();
         if (initialDir.Length() < std::numeric_limits<double>::epsilon())
        {
            initialDir = { 1.0, -1.0 };
            initialDir = initialDir.Normalize();
        }

        if (Physics::RaySegmentIntersection(laserOrigin, initialDir, shieldSegment.first, shieldSegment.second, intersectionPoint, t))
        {
            // 교차점이 레이저의 끝점(화면 중앙)보다 가까운지 확인
            double laserLength = (center - laserOrigin).Length();
            if (t > 1e-6 && t <= laserLength)
            {
                 return true;
            }
        }
    }

    return false;
}

void Laser::Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const
{
    if (showingWarningLaser)
    {
        for (const auto& segment : warningLaserPath)
        {
            if (std::isfinite(segment.first.x) && std::isfinite(segment.first.y) && std::isfinite(segment.second.x) && std::isfinite(segment.second.y))
            {
                renderer.DrawLine(camera_matrix * segment.first, camera_matrix * segment.second, warningLaserColor, warningLaserWidth);
            }
        }
    }

    if (isLaserOn)
    {
        for (const auto& segment : laserPath)
        {
            if (std::isfinite(segment.first.x) && std::isfinite(segment.first.y) && std::isfinite(segment.second.x) && std::isfinite(segment.second.y))
            {
                renderer.DrawLine(camera_matrix * segment.first, camera_matrix * segment.second, laserColor, mainLaserWidth);
            }
        }
    }
}