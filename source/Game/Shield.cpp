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
    // 쉴드가 얼어있으면 입력을 받지 않음
    if (isShieldFrozen)
    {
        return;
    }

    auto&       input          = Engine::GetInput();
    Math::vec2  mouseScreenPos = input.GetMousePosition();
    Math::ivec2 winSize        = Engine::GetWindow().GetSize();
    Math::vec2  winSizeVec     = { static_cast<double>(winSize.x), static_cast<double>(winSize.y) };

    // 카메라 위치 가져오기 (월드 좌표 변환을 위해)
    auto       camera    = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>();
    Math::vec2 cameraPos = camera ? camera->GetPosition() : Math::vec2{ 0, 0 };

    // 화면 중앙 좌표 (플레이어가 화면 중앙이나 특정 위치에 있다고 가정할 때의 기준점)
    // 주의: 현재 카메라는 플레이어를 화면 하단 1/4 지점에 둡니다.
    // 더 정확한 조준을 위해 '플레이어의 화면상 위치'를 기준으로 계산하는 것이 좋으나,
    // 여기서는 마우스의 월드 좌표를 직접 계산합니다.

    // 마우스 좌표를 월드 좌표로 변환
    // (Screen -> NDC -> World 역변환 대신, 간단히 카메라 위치를 더하는 방식으로 근사)
    // 엔진의 좌표계(Y-Up)와 SDL 마우스(Y-Down) 차이 보정
    // 화면 좌하단이 (0,0)인 OpenGL 좌표계 기준 마우스 위치:
    Math::vec2 mouseGLPos = { mouseScreenPos.x, winSizeVec.y - mouseScreenPos.y };

    // 카메라의 좌하단 월드 좌표
    // 카메라의 Position은 화면 중앙을 가리키므로, 좌하단은:
    Math::vec2 cameraBottomLeft = cameraPos - (winSizeVec * 0.5);

    // 마우스의 실제 월드 좌표
    Math::vec2 mouseWorldPos = cameraBottomLeft + mouseGLPos;

    // 플레이어(쉴드 주인)에서 마우스를 향하는 벡터
    Math::vec2 dir = mouseWorldPos - owner->GetPosition();

    // 아크탄젠트로 각도 계산하여 쉴드 회전
    shieldAngle = std::atan2(dir.y, dir.x);


    // [[ 2. 우클릭 패링 적용 ]]
    if (input.MouseButtonJustPressed(CS230::Input::MouseButton::Right))
    {
        // 패링 타이밍 윈도우가 활성화된 상태라면 패리 시도
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
    return !isShieldFrozen && Engine::GetInput().KeyDown(CS230::Input::Keys::Space);
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