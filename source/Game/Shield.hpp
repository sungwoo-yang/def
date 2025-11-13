#pragma once
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Component.hpp"
#include "Engine/Vec2.hpp"
#include <array>
#include <utility>
#include <vector>

namespace CS230
{
    class GameObject;
}

namespace CS230
{
    class IRenderer2D;
}

namespace Math
{
    class TransformationMatrix;
}

class Shield : public CS230::Component
{
public:
    Shield(CS230::GameObject* owner); // 생성자 (쉴드는 소유자의 위치를 따라다님)

    void Update(double dt) override;
    void HandleInput(double dt); // 회전, 패리
    void Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const;
    void UpdatePosition();             // 쉴드 위치 갱신
    void HandleHit(bool parrySuccess); // 패리 성공 여부

    void SetParryWindowActive(bool active) // 레이저로부터 패리 가능 상태인지 확인
    {
        parryWindowActive = active;
    }

    bool IsParryWindowActive() const // 패리 가능 상태 여부 확인
    {
        return parryWindowActive;
    }

    void                                           TryParry();          // 패리 시도
    bool                                           ConsumeParryState(); // 패리 성공 여부
    std::vector<std::pair<Math::vec2, Math::vec2>> GetSegments() const; // 물리 계산을 위한 선분 정보.

    // ImGui
    bool         isShieldFrozen       = false;
    double       shieldFrozenTimer    = 0.0;
    const double shieldFreezeDuration = 3.0;

private:
    void UpdateShieldColor(double dt);

    CS230::GameObject* owner; // 쉴드의 소유자 (Player)

    // 쉴드 기하 정보
    double     shieldAngle  = PI / 2.0;
    double     shieldLength = 100.0;
    Math::vec2 shieldStart;
    Math::vec2 shieldEnd;

    // 쉴드 색상 및 상태
    CS200::RGBA          shieldColor = CS200::CYAN;
    std::array<float, 4> currentShieldColor{};
    std::array<float, 4> targetShieldColor{};

    // 피격 및 복구 타이머
    double       shieldHitTimer          = 0.0;
    const double shieldColorRecoveryTime = 1.0;

    // 패링 상태
    bool isParrying        = false; // 'Space'를 누른 순간 true
    bool parryWindowActive = false; // 레이저가 알려주는 패리 가능 시간
};