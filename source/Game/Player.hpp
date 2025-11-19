#pragma once

#include "Engine/Dash.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include <memory>
#include <optional>
#include <string>
#include <vector>

class Shield; 

namespace CS200
{
    class IRenderer2D;
}

class Player : public CS230::GameObject
{
public:
    Player(Math::vec2 start_pos);
    ~Player() override = default;

    void Update(double dt) override;
    void Draw(const Math::TransformationMatrix& camera_matrix) override;
    void DrawImGui() override;
    
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

    void ResetState();
    void SetSavePoint(Math::vec2 new_spawn_point);

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

    CS230::GameObject* interactionTarget;
    bool               isInteracting;

private:
    void HandleInput(double dt);

    Shield* shieldComponent;

    const double gravity;
    const double jumpStrength;
    const double baseSpeed;
    const double sprintSpeedMultiplier;
    const double sprintActivationTime;

    Math::vec2 startPosition;
    Math::vec2 previousPosition;

    double       jumpBufferTimer = 0.0;
    double       coyoteTimer     = 0.0;
    const double jumpBufferTime  = 0.1;
    const double coyoteTime      = 0.1;
};