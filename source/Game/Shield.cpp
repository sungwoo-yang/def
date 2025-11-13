#include "Game/Shield.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Matrix.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept> // std::runtime_error

// DemoReflection.cpp의 'ease_color_to_target' 헬퍼 함수
namespace
{
    template <typename FLOAT = double>
    void ease_color_to_target(std::array<float, 4>& current, const std::array<float, 4>& target, FLOAT delta_time, FLOAT weight = 1.0)
    {
        // std::array에 대한 연산자 오버로딩 (DemoReflection.cpp에 있던 것)
        auto subtract = [](const std::array<float, 4>& a, const std::array<float, 4>& b) -> std::array<float, 4>
        {
            return { a[0] - b[0], a[1] - b[1], a[2] - b[2], a[3] - b[3] };
        };
        auto multiply = [](FLOAT scalar, const std::array<float, 4>& arr) -> std::array<float, 4>
        {
            const auto s = static_cast<float>(scalar);
            return { s * arr[0], s * arr[1], s * arr[2], s * arr[3] };
        };
        auto add_assign = [&](std::array<float, 4>& a, const std::array<float, 4>& b) -> std::array<float, 4>&
        {
            a[0] += b[0]; a[1] += b[1]; a[2] += b[2]; a[3] += b[3];
            return a;
        };

        const auto easing = std::min(delta_time * weight, static_cast<FLOAT>(1.0));
        add_assign(current, multiply(easing, subtract(target, current)));
    }
} // 익명 네임스페이스 끝

Shield::Shield(CS230::GameObject* owner)
    : owner(owner), shieldHitTimer(shieldColorRecoveryTime)
{
    if (owner == nullptr)
    {
        throw std::runtime_error("Shield component must have a valid owner.");
    }
    
    // 색상 초기화
    currentShieldColor = CS200::unpack_color(COLOR_CYAN);
    targetShieldColor  = CS200::unpack_color(COLOR_CYAN);

    UpdatePosition(); // 소유자 위치 기준으로 쉴드 초기 위치 설정
}

void Shield::HandleInput(double dt)
{
    // 쉴드가 얼어있으면 입력을 받지 않음
    if (isShieldFrozen)
    {
        return;
    }

    auto& input = CS230::Engine::GetInput();
    
    // 쉴드 회전 (방향키)
    const double rotateSpeed = PI / 2.0;
    if (input.KeyDown(CS230::Input::Keys::Left))
        shieldAngle += rotateSpeed * dt;
    if (input.KeyDown(CS230::Input::Keys::Right))
        shieldAngle -= rotateSpeed * dt;
    
    // 각도 정규화 (0 ~ 2*PI)
    shieldAngle = fmod(shieldAngle, 2.0 * PI);
    if (shieldAngle < 0)
        shieldAngle += 2.0 * PI;

    // 패리 시도 (스페이스바)
    if (parryWindowActive && input.KeyJustPressed(CS230::Input::Keys::Space))
    {
        TryParry();
    }
}

void Shield::TryParry()
{
    // 패리 윈도우가 활성화되어 있고, 쉴드가 얼지 않았을 때만 성공
    if (parryWindowActive && !isShieldFrozen)
    {
        isParrying = true;
        CS230::Engine::GetLogger().LogEvent("Parry Input Success!");
    }
}

bool Shield::ConsumeParryState()
{
    // 레이저가 켜지는 순간 이 함수를 호출하여 패리 시도 여부를 확인
    if (isParrying)
    {
        isParrying = false; // 상태를 "소모"함 (다음 프레임에 다시 false)
        return true;
    }
    return false;
}

void Shield::Update(double dt)
{
    // 1. 쉴드 위치를 매 프레임 소유자(Player) 위치 기준으로 갱신
    UpdatePosition();

    // 2. 쉴드 냉각 타이머 업데이트
    if (isShieldFrozen)
    {
        shieldFrozenTimer += dt;
        if (shieldFrozenTimer >= shieldFreezeDuration)
        {
            isShieldFrozen    = false;
            shieldFrozenTimer = 0.0;
            CS230::Engine::GetLogger().LogEvent("Shield Unfrozen.");
        }
    }

    // 3. 쉴드 색상 복구 로직
    UpdateShieldColor(dt);

    // 4. 패리 윈도우가 활성화되지 않았다면, 패리 시도 상태(isParrying)를 강제 리셋
    if (!parryWindowActive)
    {
        isParrying = false;
    }
}

void Shield::UpdatePosition()
{
    const Math::vec2 ownerPos = owner->GetPosition();
    double dx = (shieldLength / 2.0) * std::cos(shieldAngle);
    double dy = (shieldLength / 2.0) * std::sin(shieldAngle);
    shieldStart = ownerPos + Math::vec2{ dx, dy };
    shieldEnd   = ownerPos - Math::vec2{ dx, dy };
}

void Shield::Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const
{
    // 카메라 매트릭스가 적용된 좌표로 라인을 그림
    renderer.DrawLine(camera_matrix * shieldStart, camera_matrix * shieldEnd, shieldColor, 3.0);
}

void Shield::HandleHit(bool parrySuccess)
{
    if (parrySuccess)
    {
        // 패리 성공: 쉴드 고정
        isShieldFrozen    = true;
        shieldFrozenTimer = 0.0;
        CS230::Engine::GetLogger().LogEvent("Shield Frozen!");
    }
    else
    {
        // 패리 실패 (피격): 쉴드 색상 변경
        targetShieldColor = CS200::unpack_color(COLOR_RED);
        shieldHitTimer    = 0.0;
        CS230::Engine::GetLogger().LogEvent("Shield hit by RED laser!");
    }
}

std::vector<std::pair<Math::vec2, Math::vec2>> Shield::GetSegments() const
{
    return { { shieldStart, shieldEnd } };
}

void Shield::UpdateShieldColor(double dt)
{
    // 쉴드가 빨간색(피격 상태)인지 확인
    bool isTargetRed = (targetShieldColor[0] > 0.9f && targetShieldColor[1] < 0.1f && targetShieldColor[2] < 0.1f);
    
    if (isTargetRed)
    {
        // 빨간색이면, 복구 시간까지 타이머 증가
        shieldHitTimer += dt;
        if (shieldHitTimer >= shieldColorRecoveryTime)
        {
            // 복구 시간이 되면 기본 색상(시안)으로 타겟 변경
            targetShieldColor = CS200::unpack_color(COLOR_CYAN);
        }
    }
    else
    {
        shieldHitTimer = shieldColorRecoveryTime;
        // 타겟 색상이 시안이 아니라면 (예: 초기화, 패리 성공 등)
        if (!(targetShieldColor[0] < 0.1f && targetShieldColor[1] > 0.9f && targetShieldColor[2] > 0.9f))
        {
            targetShieldColor = CS200::unpack_color(COLOR_CYAN);
        }
    }
    
    // 현재 색상(currentShieldColor)을 목표 색상(targetShieldColor)으로 서서히 보간
    ease_color_to_target(currentShieldColor, targetShieldColor, dt, 5.0);
    shieldColor = CS200::pack_color(currentShieldColor);
}