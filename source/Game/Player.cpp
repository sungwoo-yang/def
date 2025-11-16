#include "Player.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "Shield.hpp"
#include "WorldTextManager.hpp"
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

Player::Player(Math::vec2 start_pos)
    : CS230::GameObject(start_pos), startPosition(start_pos), previousPosition(start_pos), shieldComponent(nullptr), isSprinting(false), shiftHoldTimer(0.0), isJumping(true),
      currentPlatformIndex(std::nullopt), velocityY(0.0), faceRight(true), interactionTarget(nullptr), gravity(1500.0), jumpStrength(700.0), baseSpeed(300.0), sprintSpeedMultiplier(1.8),
      sprintActivationTime(0.5)
{
    shieldComponent = new Shield(this);
    AddGOComponent(shieldComponent);

    AddGOComponent(new CS230::RectCollision(PLAYER_COLLISION_BOX, this));

    dashComponent.dashCooldown = 1.0;
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
    interactionTarget = nullptr;
    previousPosition  = GetPosition();

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

    GameObject::Update(dt);
}

void Player::HandleInput(double dt)
{
    auto& input = Engine::GetInput();

    if (shieldComponent)
    {
        shieldComponent->HandleInput(dt);
    }

    if (input.KeyJustPressed(CS230::Input::Keys::F))
    {
        if (interactionTarget != nullptr)
        {
            // 상호작용 대상이 있다면 Interact 함수 호출
            interactionTarget->Interact(this);
        }
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
}

bool Player::CanCollideWith(GameObjectTypes other_object_type)
{
    if (other_object_type == GameObjectTypes::Floor)
    {
        return true;
    }
    else if (other_object_type == GameObjectTypes::Sign)
    {
        return true;
    }
    else if (other_object_type == GameObjectTypes::Bonfire)
    {
        return true;
    }

    return false;
}

void Player::ResolveCollision(CS230::GameObject* other_object)
{
    // WorldTextManager를 가져옵니다.
    auto textManager = Engine::GetGameStateManager().GetGSComponent<WorldTextManager>();

    // 1. 충돌한 오브젝트의 타입을 확인
    if (other_object->Type() == GameObjectTypes::Floor)
    {
        // 2. 바닥(Floor)인 경우: 기존의 착지 및 측면 충돌 로직 수행
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

        // 수직 충돌 (착지)
        if (velocityY <= 0 && was_above && is_below_or_on && my_box.Right() > other_box.Left() && my_box.Left() < other_box.Right()) // 수평 겹침 확인
        {
            SetPosition({ GetPosition().x, platform_top + (PLAYER_COLLISION_SIZE.y / 2.0) });
            velocityY = 0.0;
            isJumping = false;
        }
        // 수평 충돌 (측면)
        else if (!was_above) // 위에서 떨어진 게 아니라면 (즉, 옆에서 왔다면)
        {
            double overlap_left  = my_box.Right() - other_box.Left();
            double overlap_right = other_box.Right() - my_box.Left();

            if (overlap_left < overlap_right)
                SetPosition({ GetPosition().x - overlap_left, GetPosition().y });
            else
                SetPosition({ GetPosition().x + overlap_right, GetPosition().y });


            SetVelocity({ 0.0, GetVelocity().y });
        }
    }
    else if (other_object->Type() == GameObjectTypes::Sign || other_object->Type() == GameObjectTypes::Bonfire)
    {
        // 3. 표지판(Sign) 또는 모닥불(Bonfire)인 경우:
        //    물리적으로 밀어내지 않고, interactionTarget으로만 설정
        interactionTarget = other_object;

        // "Press 'F'" 프롬프트를 오브젝트 '아래'에 요청
        if (textManager != nullptr)
        {
            textManager->ShowTextBelow(other_object, "Press 'F'");
        }
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