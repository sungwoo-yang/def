#pragma once

#include "Engine/Dash.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

// 전방 선언
class Shield; // 쉴드 컴포넌트

namespace CS200
{
    class IRenderer2D;
}

/**
 * \brief DemoAstar의 플랫폼 이동 기능과 Dash/Sprint를 담당하는 플레이어 클래스입니다.
 *
 * 쉴드 기능은 'Shield' 컴포넌트로 분리되었습니다.
 */
class Player : public CS230::GameObject
{
public:
    Player(Math::vec2 start_pos);
    ~Player() override = default;

    // GameObject 가상 함수 오버라이드
    void Update(double dt) override;
    void Draw(const Math::TransformationMatrix& camera_matrix) override;

    const Math::vec2& GetPosition() const
    {
        return GameObject::GetPosition();
    };

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Player;
    }

    std::string TypeName() override
    {
        return "Player";
    }

    bool CanCollideWith(GameObjectTypes other_object_type) override;
    void ResolveCollision(CS230::GameObject* other_object) override;

    void UpdateAnimation(double dt);
    void ResetState();

    Shield* GetShield() const
    {
        return shieldComponent;
    }

    CS230::DashComponent  dashComponent;
    bool                  isSprinting          = false;
    double                shiftHoldTimer       = 0.0;
    bool                  isJumping            = false;
    std::optional<size_t> currentPlatformIndex = std::nullopt;
    double                velocityY            = 0.0;
    bool                  faceRight            = true;

private:
    void HandleInput(double dt);

    // 컴포넌트
    Shield* shieldComponent;

    // 물리 상태
    double       gravity               = 1500.0;
    double       jumpStrength          = 700.0;
    const double baseSpeed             = 300.0;
    const double sprintSpeedMultiplier = 1.8;
    const double sprintActivationTime  = 0.5;
    Math::vec2   startPosition;
};