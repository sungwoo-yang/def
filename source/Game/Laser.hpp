#pragma once

#include "Engine/Vec2.hpp"
#include "CS200/RGBA.hpp"
#include <vector>
#include <utility>

namespace CS200
{
    class IRenderer2D;
}

class Target; // Target 클래스 전방 선언

/**
 * \brief DemoReflection의 레이저 로직을 관리하는 클래스입니다.
 *
 * 레이저의 발사 타이밍, 경고, 경로 계산, 그리기를 담당합니다.
 */
class Laser
{
public:
    Laser(Math::vec2 origin);

    /**
     * \brief 레이저 상태를 업데이트합니다.
     * \param dt Delta time
     * \param playerIsParrying 플레이어가 현재 패링 중인지 여부
     * \param reflectionSegments 레이저가 반사될 수 있는 모든 표면 (예: 플레이어 쉴드)
     * \return 패리 윈도우가 활성화되었는지 여부를 반환합니다.
     */
    bool Update(double dt, bool playerIsParrying, const std::vector<std::pair<Math::vec2, Math::vec2>>& reflectionSegments);

    void Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const;

    bool IsLaserOn() const { return isLaserOn; }
    bool IsParried() const { return laserColor == COLOR_YELLOW; }
    const std::vector<std::pair<Math::vec2, Math::vec2>>& GetLaserPath() const { return laserPath; }

    /**
     * \brief 플레이어가 레이저에 맞았는지 확인합니다. (패링이 아닐 때)
     * \param shieldSegments 플레이어 쉴드 세그먼트
     * \return 쉴드에 맞았는지 여부
     */
    bool CheckShieldCollision(const std::vector<std::pair<Math::vec2, Math::vec2>>& shieldSegments) const;

    /**
     * \brief 패링이 성공했는지 확인합니다. (레이저가 켜지는 순간)
     * \param playerIsParrying 플레이어가 패링 입력을 했는지 여부
     * \return 패링 성공 여부
     */
    bool CheckParrySuccess(bool playerIsParrying);


private:
    void CalculateLaser(const std::vector<std::pair<Math::vec2, Math::vec2>>& reflectionSegments);

    Math::vec2                                     laserOrigin;
    std::vector<std::pair<Math::vec2, Math::vec2>> laserPath;
    std::vector<std::pair<Math::vec2, Math::vec2>> warningLaserPath;
    
    double       laserTimer          = 0.0;
    bool         isLaserOn           = false;
    bool         wasLaserOn          = false; // 레이저가 켜지는 순간을 감지
    bool         showingWarningLaser = false;
    CS200::RGBA  warningLaserColor   = COLOR_WARNING;
    CS200::RGBA  laserColor          = COLOR_RED;

    // 타이밍 상수
    const double warningLaserLeadTime  = 5.0;
    const double parryWindowDuration   = 0.5;
    const double parryWindowStartTimeOffset = warningLaserLeadTime - parryWindowDuration;
    const double laserCycleTime        = 10.0;
    const double laserOnDuration       = 2.5;

    // 그리기 상수
    const double mainLaserWidth    = 5.0;
    const double warningLaserWidth = 1.0;
};