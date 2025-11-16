#include "Game/Player.hpp"
#include "Engine/GameObject.hpp"
#include "Game/Shield.hpp"
// #include "Engine/Sprite.hpp" // --- 스프라이트 제거 ---
#include "CS200/IRenderer2D.hpp" // <--- 렌더러 포함
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace
{
    constexpr Math::irect PLAYER_COLLISION_BOX{
        { -20, -60 },
        {  20,  60 }
    };
    constexpr Math::vec2 PLAYER_COLLISION_SIZE{ 40.0, 120.0 };
}

Player::Player(Math::vec2 start_pos) : CS230::GameObject(start_pos), startPosition(start_pos)
{
    shieldComponent = new Shield(this);
    AddGOComponent(shieldComponent);

    AddGOComponent(new CS230::RectCollision(PLAYER_COLLISION_BOX, this));

    dashComponent.dashCooldown = 1.0;
    ResetState();
    previousPosition = start_pos;
}

void Player::ResetState()
{
    SetPosition(startPosition);
    previousPosition     = startPosition;
    velocityY            = 0.0;
    isJumping            = true;
    currentPlatformIndex = std::nullopt;
    faceRight            = true;

    dashComponent.isDashing         = false;
    dashComponent.dashTimer         = 0.0;
    dashComponent.dashCooldownTimer = 0.0;
    isSprinting                     = false;
    shiftHoldTimer                  = 0.0;
    interactionTarget               = nullptr;

    if (shieldComponent != nullptr)
    {
        RemoveGOComponent<Shield>();
    }
    shieldComponent = new Shield(this);
    AddGOComponent(shieldComponent);
}

void Player::Update(double dt)
{
    previousPosition = GetPosition();

    HandleInput(dt);
    dashComponent.UpdateTimers(dt);

    bool applyGravity = true;
    if (dashComponent.IsDashing() && dashComponent.disableGravityOnDash)
    {
        applyGravity = false;
    }
    else if (!isJumping)
    {
        applyGravity = false;
        velocityY    = 0.0;
    }

    if (applyGravity)
    {
        velocityY -= gravity * dt;
    }

    double finalVelX = GetVelocity().x;
    if (dashComponent.IsDashing())
    {
        finalVelX = dashComponent.GetDashVelocityX();
    }
    SetVelocity({ finalVelX, velocityY });

    isJumping            = true;
    currentPlatformIndex = std::nullopt;
    interactionTarget    = nullptr;

    GameObject::Update(dt);
}

void Player::HandleInput(double dt)
{
    auto& input = Engine::GetInput();

    if (shieldComponent)
    {
        shieldComponent->HandleInput(dt);
    }

    const bool shiftDown           = input.KeyDown(CS230::Input::Keys::LShift);
    const bool shiftJustReleased   = input.KeyJustReleased(CS230::Input::Keys::LShift);
    bool       canStartSpecialMove = dashComponent.dashCooldownTimer <= 0.0 && !dashComponent.IsDashing() && !isSprinting;

    if (shiftDown)
    {
        if (canStartSpecialMove)
        {
            shiftHoldTimer += dt;
            if (shiftHoldTimer >= sprintActivationTime)
            {
                isSprinting = true;
            }
        }
    }
    else if (shiftJustReleased)
    {
        if (shiftHoldTimer > 0.0 && shiftHoldTimer < sprintActivationTime)
        {
            if (canStartSpecialMove)
            {
                dashComponent.TryStartDash(faceRight);
            }
        }
        isSprinting    = false;
        shiftHoldTimer = 0.0;
    }
    else
    {
        isSprinting    = false;
        shiftHoldTimer = 0.0;
    }

    if (!dashComponent.IsDashing())
    {
        double currentBaseSpeed = baseSpeed;
        if (isSprinting)
        {
            currentBaseSpeed *= sprintSpeedMultiplier;
        }

        Math::vec2 move{ 0.0, 0.0 };
        if (input.KeyDown(CS230::Input::Keys::A))
        {
            move.x -= 1.0;
            faceRight = false;
        }
        if (input.KeyDown(CS230::Input::Keys::D))
        {
            move.x += 1.0;
            faceRight = true;
        }

        SetVelocity({ move.x * currentBaseSpeed, GetVelocity().y });

        if (!isJumping && (input.KeyJustPressed(CS230::Input::Keys::W) || input.KeyJustPressed(CS230::Input::Keys::Space)))
        {
            velocityY = jumpStrength;
            isJumping = true;
            Engine::GetLogger().LogEvent("Event: Player Jump");
        }
    }

    if (input.KeyJustPressed(CS230::Input::Keys::F))
    {
        if (interactionTarget != nullptr)
        {
            interactionTarget->Interact(this);
        }
    }
}

bool Player::CanCollideWith(GameObjectTypes other_object_type)
{
    if (other_object_type == GameObjectTypes::Floor)
    {
        return true;
    }

    if (other_object_type == GameObjectTypes::Sign)
    {
        return true;
    }
    return false;
}

void Player::ResolveCollision(CS230::GameObject* other_object)
{
    if (other_object->Type() != GameObjectTypes::Floor)
    {
        return;
    }

    CS230::RectCollision* my_collider = GetGOComponent<CS230::RectCollision>();
    if (!my_collider)
        return;

    CS230::SATCollision* other_collider = other_object->GetGOComponent<CS230::SATCollision>();
    if (!other_collider)
        return;

    Math::rect my_box    = my_collider->WorldBoundary();
    Math::rect other_box = other_collider->WorldBoundary().FindBoundary();

    double prev_bottom    = previousPosition.y - (PLAYER_COLLISION_SIZE.y / 2.0);
    double platform_top   = other_box.Top();
    bool   was_above      = prev_bottom >= platform_top;
    bool   is_below_or_on = my_box.Bottom() <= platform_top;
    if (other_object->Type() == GameObjectTypes::Floor)
    {
        if (velocityY <= 0 && was_above && is_below_or_on)
        {
            SetPosition({ GetPosition().x, platform_top + (PLAYER_COLLISION_SIZE.y / 2.0) });
            velocityY = 0.0;
            isJumping = false;
        }
        else
        {
            double overlap_left  = my_box.Right() - other_box.Left();
            double overlap_right = other_box.Right() - my_box.Left();

            if (overlap_left < overlap_right)
            {
                SetPosition({ GetPosition().x - overlap_left, GetPosition().y });
            }
            else
            {
                SetPosition({ GetPosition().x + overlap_right, GetPosition().y });
            }

            SetVelocity({ 0.0, GetVelocity().y });
        }
    }
    else if (other_object->Type() == GameObjectTypes::Sign)
    {
        interactionTarget = other_object;
    }
}

void Player::Draw(const Math::TransformationMatrix& camera_matrix)
{
    CS200::IRenderer2D&        renderer  = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(PLAYER_COLLISION_SIZE);

    renderer.DrawRectangle(transform, CS200::WHITE, CS200::CLEAR, 0.0);

    if (shieldComponent)
    {
        shieldComponent->Draw(renderer, camera_matrix);
    }

    GameObject::Draw(camera_matrix);
}