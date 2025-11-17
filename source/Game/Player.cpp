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
        { -20, -40 },
        {  20,  40 }
    };
    constexpr Math::vec2 PLAYER_COLLISION_SIZE{ 40.0, 80.0 };
}

Player::Player(Math::vec2 start_pos)
    : CS230::GameObject(start_pos), startPosition(start_pos), previousPosition(start_pos), shieldComponent(nullptr), isSprinting(false), shiftHoldTimer(0.0), isJumping(true),
      currentPlatformIndex(std::nullopt), velocityY(0.0), faceRight(true), interactionTarget(nullptr), isInteracting(false), gravity(1500.0), jumpStrength(700.0), baseSpeed(300.0),
      sprintSpeedMultiplier(1.8), sprintActivationTime(0.5)
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
    isInteracting                   = false;

    if (shieldComponent != nullptr)
    {
        RemoveGOComponent<Shield>();
    }
    shieldComponent = new Shield(this);
    AddGOComponent(shieldComponent);
}

void Player::SetSavePoint(Math::vec2 new_spawn_point)
{
    startPosition = new_spawn_point;
    Engine::GetLogger().LogEvent("Player save point updated!");
}

void Player::Update(double dt)
{
    interactionTarget = nullptr;
    previousPosition  = GetPosition();

    // [[ 1. 타이머 업데이트 ]]
    if (jumpBufferTimer > 0.0)
        jumpBufferTimer -= dt;
    if (coyoteTimer > 0.0)
        coyoteTimer -= dt;

    HandleInput(dt);
    dashComponent.UpdateTimers(dt);

    // [[ 2. 중력 적용 로직 개선 ]]
    bool applyGravity = true;
    if (dashComponent.IsDashing() && dashComponent.disableGravityOnDash)
    {
        applyGravity = false;
    }
    // isJumping 대신 coyoteTimer를 사용하여 '공중' 판정
    // (땅에 있으면 coyoteTimer가 계속 리셋됨)
    else if (coyoteTimer > 0.0)
    {
        // 땅에 붙어있거나 코요테 타임 중이면 중력 0 (미끄러짐 방지)
        // 단, 점프 시작 직후에는 중력이 적용되어야 하므로 velocityY < 0 일때만
        // ...하지만 간단히 기존 isJumping 로직을 유지하되 보조적으로 사용
    }

    if (applyGravity)
    {
        velocityY -= gravity * dt;
    }

    // ... (속도 설정 및 GameObject::Update) ...
    double finalVelX = GetVelocity().x;
    if (dashComponent.IsDashing())
    {
        finalVelX = dashComponent.GetDashVelocityX();
    }
    SetVelocity({ finalVelX, velocityY });

    // [중요] 매 프레임 '공중' 상태로 가정 (충돌 처리에서 땅에 닿으면 해제)
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

    if (input.KeyJustPressed(CS230::Input::Keys::R))
    {
        ResetState();
        Engine::GetLogger().LogEvent("Event: Player Respawned (R)");
        return;
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

        if (shieldComponent && shieldComponent->IsGuardUp())
        {
            currentBaseSpeed *= 0.1;
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
            jumpBufferTimer = jumpStrength;
        }

        if (coyoteTimer > 0.0 && jumpBufferTimer > 0.0 && velocityY <= 0.0)
        {
            velocityY = jumpStrength;
            isJumping = true;

            // 점프 사용 처리
            jumpBufferTimer = 0.0;
            coyoteTimer     = 0.0; // 즉시 공중 상태로 전환

            Engine::GetLogger().LogEvent("Event: Player Jump (Buffered/Coyote)");
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

void Player::ResolveCollision(GameObject* other_object)
{
    // WorldTextManager와 Input을 가져옵니다.
    auto  textManager = Engine::GetGameStateManager().GetGSComponent<WorldTextManager>();
    auto& input       = Engine::GetInput();

    // 1. 충돌한 오브젝트의 타입을 확인
    if (other_object->Type() == GameObjectTypes::Floor)
    {
        // 2. 바닥(Floor)인 경우: (기존 로직 - 변경 없음)
        CS230::RectCollision* my_collider = GetGOComponent<CS230::RectCollision>();
        if (!my_collider)
            return;
        CS230::SATCollision* other_collider = other_object->GetGOComponent<CS230::SATCollision>();
        if (!other_collider)
            return;

        Math::rect my_box    = my_collider->WorldBoundary();
        Math::rect other_box = other_collider->WorldBoundary().FindBoundary();

        double prev_bottom = previousPosition.y - (PLAYER_COLLISION_SIZE.y / 2.0);
        double prev_top    = previousPosition.y + (PLAYER_COLLISION_SIZE.y / 2.0);
        double prev_left   = previousPosition.x - (PLAYER_COLLISION_SIZE.x / 2.0);
        double prev_right  = previousPosition.x + (PLAYER_COLLISION_SIZE.x / 2.0);

        double platform_top    = other_box.Top();
        double platform_bottom = other_box.Bottom();
        double platform_left   = other_box.Left();
        double platform_right  = other_box.Right();

        // [2] 상태 판정 (이전 프레임 기준)
        bool was_above = prev_bottom >= platform_top;
        bool was_below = prev_top <= platform_bottom;
        bool was_left  = prev_right <= platform_left;
        bool was_right = prev_left >= platform_right;

        // [3] 겹침 깊이 계산 (현재 프레임 기준)
        double overlap_bottom = my_box.Top() - other_box.Bottom(); // 머리가 뚫고 들어간 깊이
        double overlap_top    = other_box.Top() - my_box.Bottom(); // 발이 뚫고 들어간 깊이
        double overlap_left   = my_box.Right() - other_box.Left(); // 오른쪽으로 뚫고 들어간 깊이
        double overlap_right  = other_box.Right() - my_box.Left(); // 왼쪽으로 뚫고 들어간 깊이

        // 수직/수평 겹침 여부 확인
        bool horizontal_overlap = my_box.Right() > other_box.Left() && my_box.Left() < other_box.Right();
        bool vertical_overlap   = my_box.Top() > other_box.Bottom() && my_box.Bottom() < other_box.Top();

        if (!horizontal_overlap || !vertical_overlap)
            return; // 실제 충돌 아님

        // [4] 충돌 해결 로직

        // Case 1: 착지 (Falling & Was Above)
        if (velocityY <= 0 && was_above && horizontal_overlap)
        {
            SetPosition({ GetPosition().x, platform_top + (PLAYER_COLLISION_SIZE.y / 2.0) });
            velocityY = 0.0;
            isJumping = false;

            // [[ 5. 착지 시 코요테 타임 충전 ]]
            coyoteTimer = coyoteTime;
        }
        // Case 2: 천장 충돌 (Rising & Was Below)
        else if (velocityY > 0 && was_below && horizontal_overlap)
        {
            SetPosition({ GetPosition().x, platform_bottom - (PLAYER_COLLISION_SIZE.y / 2.0) });
            velocityY = 0.0;
        }
        // Case 3: 왼쪽 벽 충돌 (Moving Right & Was Left)
        else if (GetVelocity().x > 0 && was_left && vertical_overlap)
        {
            SetPosition({ platform_left - (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y });
            SetVelocity({ 0.0, GetVelocity().y });
        }
        // Case 4: 오른쪽 벽 충돌 (Moving Left & Was Right)
        else if (GetVelocity().x < 0 && was_right && vertical_overlap)
        {
            SetPosition({ platform_right + (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y });
            SetVelocity({ 0.0, GetVelocity().y });
        }
        // Case 5: 그 외 (이미 겹쳐있거나 판정 애매함) -> MTV(최소 침투 깊이)로 밀어내기
        else
        {
            // 가장 얕은 겹침을 찾아서 그 방향으로 밀어냅니다.
            // (머리 박치기 시 overlap_bottom이 가장 작을 것이므로 아래로 밀려남)

            double min_overlap = overlap_bottom;
            int    axis        = 0; // 0: bottom, 1: top, 2: left, 3: right

            if (overlap_top < min_overlap)
            {
                min_overlap = overlap_top;
                axis        = 1;
            }
            if (overlap_left < min_overlap)
            {
                min_overlap = overlap_left;
                axis        = 2;
            }
            if (overlap_right < min_overlap)
            {
                min_overlap = overlap_right;
                axis        = 3;
            }

            switch (axis)
            {
                case 0: // 위로 뚫음 -> 아래로 밀기 (천장)
                    SetPosition({ GetPosition().x, platform_bottom - (PLAYER_COLLISION_SIZE.y / 2.0) });
                    if (velocityY > 0)
                        velocityY = 0.0;
                    break;
                case 1: // 아래로 뚫음 -> 위로 밀기 (바닥)
                    SetPosition({ GetPosition().x, platform_top + (PLAYER_COLLISION_SIZE.y / 2.0) });
                    if (velocityY < 0)
                        velocityY = 0.0;
                    isJumping = false;

                    // [[ 5. 착지 시 코요테 타임 충전 ]]
                    coyoteTimer = coyoteTime;
                    break;
                case 2: // 오른쪽으로 뚫음 -> 왼쪽으로 밀기
                    SetPosition({ platform_left - (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y });
                    break;
                case 3: // 왼쪽으로 뚫음 -> 오른쪽으로 밀기
                    SetPosition({ platform_right + (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y });
                    break;
            }
        }
    }
    else if (other_object->Type() == GameObjectTypes::Sign || other_object->Type() == GameObjectTypes::Bonfire)
    {
        // 3. 표지판(Sign) 또는 모닥불(Bonfire)인 경우:
        interactionTarget = other_object; // 상호작용 대상 설정

        // F키가 방금 눌렸는지 확인
        if (input.KeyJustPressed(CS230::Input::Keys::F))
        {
            isInteracting = true; // 상호작용 상태 시작
        }

        if (isInteracting)
        {
            other_object->Interact(this);
        }
        else
        {
            if (textManager != nullptr)
            {
                // 프롬프트 표시 (흰색, 월드 좌표 추적)
                textManager->ShowTextBelow(other_object, "Press 'F'", 0.4, CS200::WHITE);
            }
        }
    }
}

void Player::Draw(const Math::TransformationMatrix& camera_matrix)
{
    CS200::IRenderer2D&        renderer  = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(PLAYER_COLLISION_SIZE);

    renderer.DrawRectangle(transform, CS200::GREEN, CS200::CLEAR, 0.0);

    if (shieldComponent)
    {
        shieldComponent->Draw(renderer, camera_matrix);
    }

    CS230::GameObject::Draw(camera_matrix);
}