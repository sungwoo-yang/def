#include "Game/Shield.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Window.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>

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
            a[0] += b[0];
            a[1] += b[1];
            a[2] += b[2];
            a[3] += b[3];
            return a;
        };

        const auto easing = std::min(delta_time * weight, static_cast<FLOAT>(1.0));
        add_assign(current, multiply(easing, subtract(target, current)));
    }
} // 익명 네임스페이스 끝

Shield::Shield(CS230::GameObject* owner) : owner(owner), shieldHitTimer(shieldColorRecoveryTime)
{
    if (owner == nullptr)
    {
        throw std::runtime_error("Shield component must have a valid owner.");
    }

    // 색상 초기화
    currentShieldColor = CS200::unpack_color(CS200::CYAN);
    targetShieldColor  = CS200::unpack_color(CS200::CYAN);

    UpdatePosition(); // 소유자 위치 기준으로 쉴드 초기 위치 설정
}

void Shield::HandleInput([[maybe_unused]] double dt)
{
    if (isShieldFrozen)
        return;

    auto& input = Engine::GetInput();

    // --- 마우스 회전 로직 (기존과 동일) ---
    Math::vec2  mouseScreenPos = input.GetMousePosition();
    Math::ivec2 winSize        = Engine::GetWindow().GetSize();
    Math::vec2  winSizeVec     = { static_cast<double>(winSize.x), static_cast<double>(winSize.y) };

    auto       camera    = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>();
    Math::vec2 cameraPos = camera ? camera->GetPosition() : Math::vec2{ 0, 0 };

    // OpenGL 좌표계(좌하단 0,0)로 변환
    Math::vec2 mouseGLPos = { mouseScreenPos.x, winSizeVec.y - mouseScreenPos.y };

    // 카메라 중심이 아닌, 실제 카메라가 비추는 영역의 좌하단 좌표 계산
    // (카메라 위치는 화면 중심을 의미함)
    Math::vec2 cameraBottomLeft = cameraPos - (winSizeVec * 0.5);
    Math::vec2 mouseWorldPos    = cameraBottomLeft + mouseGLPos;

    Math::vec2 dir = mouseWorldPos - owner->GetPosition();
    shieldAngle    = std::atan2(dir.y, dir.x);


    // [[ 2. 우클릭 패링 시도 (누르는 순간) ]]
    if (input.MouseButtonJustPressed(CS230::Input::MouseButton::Right))
    {
        if (parryWindowActive)
        {
            TryParry();
        }
    }
}

void Shield::TryParry()
{
    if (parryWindowActive && !isShieldFrozen)
    {
        isParrying = true;
        Engine::GetLogger().LogEvent("Parry Input Success!");
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

bool Shield::IsGuardUp() const
{
    // 쉴드가 얼지 않았고(쿨타임 아님), 스페이스바를 누르고 있으면 가드 상태로 판정
    return !isShieldFrozen && Engine::GetInput().MouseButtonDown(CS230::Input::MouseButton::Right);
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
            Engine::GetLogger().LogEvent("Shield Unfrozen.");
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
    double           dx       = (shieldLength / 2.0) * std::cos(shieldAngle);
    double           dy       = (shieldLength / 2.0) * std::sin(shieldAngle);
    shieldStart               = ownerPos + Math::vec2{ dx, dy };
    shieldEnd                 = ownerPos - Math::vec2{ dx, dy };
}

void Shield::Draw(CS200::IRenderer2D& renderer, const Math::TransformationMatrix& camera_matrix) const
{
    if (isShieldFrozen || IsGuardUp())
    {
        renderer.DrawLine(camera_matrix * shieldStart, camera_matrix * shieldEnd, shieldColor, 3.0);
    }
}

void Shield::HandleHit(bool parrySuccess)
{
    if (parrySuccess)
    {
        // 패리 성공: 쉴드 고정
        isShieldFrozen    = true;
        shieldFrozenTimer = 0.0;
        Engine::GetLogger().LogEvent("Shield Frozen!");
    }
    else
    {
        // 패리 실패 (피격): 쉴드 색상 변경
        targetShieldColor = CS200::unpack_color(CS200::RED);
        shieldHitTimer    = 0.0;
        Engine::GetLogger().LogEvent("Shield hit by RED laser!");
    }
}

std::vector<std::pair<Math::vec2, Math::vec2>> Shield::GetSegments() const
{
    return {
        { shieldStart, shieldEnd }
    };
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
            targetShieldColor = CS200::unpack_color(CS200::CYAN);
        }
    }
    else
    {
        shieldHitTimer = shieldColorRecoveryTime;
        // 타겟 색상이 시안이 아니라면 (예: 초기화, 패리 성공 등)
        if (!(targetShieldColor[0] < 0.1f && targetShieldColor[1] > 0.9f && targetShieldColor[2] > 0.9f))
        {
            targetShieldColor = CS200::unpack_color(CS200::CYAN);
        }
    }

    // 현재 색상(currentShieldColor)을 목표 색상(targetShieldColor)으로 서서히 보간
    ease_color_to_target(currentShieldColor, targetShieldColor, dt, 5.0);
    shieldColor = CS200::pack_color(currentShieldColor);
}