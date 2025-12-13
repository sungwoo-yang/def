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
#include <imgui.h>
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
    playerHp                        = maxPlayerHp;
    healthState                     = HealthState::Full;
    recoverDelayTimer               = 0.0;
    tookDamageThisFrame             = false;
    invincibilityTimer              = 0.0;

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

    if (invincibilityTimer > 0.0)
    {
        invincibilityTimer -= dt;
    }

    if (jumpBufferTimer > 0.0)
        jumpBufferTimer -= dt;
    if (coyoteTimer > 0.0)
        coyoteTimer -= dt;

    HandleInput(dt);
    dashComponent.UpdateTimers(dt);

    bool applyGravity = true;
    if (dashComponent.IsDashing() && dashComponent.disableGravityOnDash)
    {
        applyGravity = false;
    }
    else if (coyoteTimer > 0.0)
    {
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
    UpdateHealthState(dt);
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

            jumpBufferTimer = 0.0;
            coyoteTimer     = 0.0;

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
    else if (other_object_type == GameObjectTypes::Door)
    {
        return true;
    }

    return false;
}

void Player::ResolveCollision(GameObject* other_object)
{
    auto  textManager = Engine::GetGameStateManager().GetGSComponent<WorldTextManager>();
    auto& input       = Engine::GetInput();

    if (other_object->Type() == GameObjectTypes::Floor)
    {
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

        bool was_above = prev_bottom >= platform_top;
        bool was_below = prev_top <= platform_bottom;
        bool was_left  = prev_right <= platform_left;
        bool was_right = prev_left >= platform_right;

        double overlap_bottom = my_box.Top() - other_box.Bottom();
        double overlap_top    = other_box.Top() - my_box.Bottom();
        double overlap_left   = my_box.Right() - other_box.Left();
        double overlap_right  = other_box.Right() - my_box.Left();

        bool horizontal_overlap = my_box.Right() > other_box.Left() && my_box.Left() < other_box.Right();
        bool vertical_overlap   = my_box.Top() > other_box.Bottom() && my_box.Bottom() < other_box.Top();

        if (!horizontal_overlap || !vertical_overlap)
            return;


        if (velocityY <= 0 && was_above && horizontal_overlap)
        {
            SetPosition({ GetPosition().x, platform_top + (PLAYER_COLLISION_SIZE.y / 2.0) });
            velocityY = 0.0;
            isJumping = false;

            coyoteTimer = coyoteTime;
        }
        else if (velocityY > 0 && was_below && horizontal_overlap)
        {
            SetPosition({ GetPosition().x, platform_bottom - (PLAYER_COLLISION_SIZE.y / 2.0) });
            velocityY = 0.0;
        }
        else if (GetVelocity().x > 0 && was_left && vertical_overlap)
        {
            SetPosition({ platform_left - (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y });
            SetVelocity({ 0.0, GetVelocity().y });
        }
        else if (GetVelocity().x < 0 && was_right && vertical_overlap)
        {
            SetPosition({ platform_right + (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y });
            SetVelocity({ 0.0, GetVelocity().y });
        }
        else
        {
            double min_overlap = overlap_bottom;
            int    axis        = 0;

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
                case 0:
                    SetPosition({ GetPosition().x, platform_bottom - (PLAYER_COLLISION_SIZE.y / 2.0) });
                    if (velocityY > 0)
                        velocityY = 0.0;
                    break;
                case 1:
                    SetPosition({ GetPosition().x, platform_top + (PLAYER_COLLISION_SIZE.y / 2.0) });
                    if (velocityY < 0)
                        velocityY = 0.0;
                    isJumping = false;

                    coyoteTimer = coyoteTime;
                    break;
                case 2: SetPosition({ platform_left - (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y }); break;
                case 3: SetPosition({ platform_right + (PLAYER_COLLISION_SIZE.x / 2.0), GetPosition().y }); break;
            }
        }
    }
    else if (other_object->Type() == GameObjectTypes::Sign || other_object->Type() == GameObjectTypes::Bonfire || other_object->Type() == GameObjectTypes::Door)
    {
        interactionTarget = other_object;

        if (input.KeyJustPressed(CS230::Input::Keys::F))
        {
            isInteracting = true;
        }

        if (isInteracting)
        {
            other_object->Interact(this);
        }
        else
        {
            if (textManager != nullptr)
            {
                textManager->ShowTextBelow(other_object, "Press 'F'", 0.5, CS200::WHITE);
            }
        }
    }
}

void Player::Draw(const Math::TransformationMatrix& camera_matrix)
{
    CS200::IRenderer2D&        renderer  = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(PLAYER_COLLISION_SIZE);
    CS200::RGBA                playerColor = CS200::GREEN;

    if (invincibilityTimer > 0.0)
    {
        // Blink effect: toggle visibility every 0.1 seconds
        if (static_cast<int>(invincibilityTimer * 10.0) % 2 == 0)
        {
            // Set alpha to 100 (approx 0x64) for semi-transparency
            // CS200::RGBA is 0xRRGGBBAA
            playerColor = (playerColor & 0xFFFFFF00) | 100;
        }
    }

    switch (healthState)
    {
    case HealthState::Full:
        playerColor = CS200::GREEN;
        break;
    case HealthState::Healthy:
        playerColor = CS200::CYAN;
        break;
    case HealthState::Hurt:
        playerColor = CS200::YELLOW;
        break;
    case HealthState::Critical:
        playerColor = 0xFFA500FF; // Orange
        break;
    case HealthState::NearDeath:
        playerColor = CS200::RED;
        break;
    case HealthState::Dead:
        playerColor = 0x8B0000FF; // Dark Red
        break;
    default:
        playerColor = CS200::GREEN;
        break;
    }

    renderer.DrawRectangle(transform, playerColor, CS200::CLEAR, 0.0);

    if (shieldComponent)
    {
        shieldComponent->Draw(renderer, camera_matrix);
    }

    CS230::GameObject::Draw(camera_matrix);
}

void Player::DrawImGui()
{
    ImGui::PushID(this);
    if (ImGui::TreeNode("Player"))
    {
        // 위치 및 속도 조작 (Double -> Float 변환 필요)
        Math::vec2 pos  = GetPosition();
        float      p[2] = { static_cast<float>(pos.x), static_cast<float>(pos.y) };
        if (ImGui::DragFloat2("Position", p))
        {
            SetPosition({ p[0], p[1] });
        }

        Math::vec2 vel  = GetVelocity();
        float      v[2] = { static_cast<float>(vel.x), static_cast<float>(vel.y) };
        if (ImGui::DragFloat2("Velocity", v))
        {
            SetVelocity({ v[0], v[1] });
            velocityY = v[1]; // Player 내부 변수도 업데이트
        }
        
        ImGui::Separator();
        ImGui::Text("Player HP: %.2f / %.2f", playerHp, maxPlayerHp);

        const char* stateItems[] = { "Dead", "Near Death", "Critical", "Hurt", "Healthy", "Full" };
        int currentItem = 0;
        int hpInt = static_cast<int>(playerHp + 0.001);

        if (hpInt >= 5)      currentItem = 5;
        else if (hpInt == 4) currentItem = 4;
        else if (hpInt == 3) currentItem = 3;
        else if (hpInt == 2) currentItem = 2;
        else if (hpInt == 1) currentItem = 1;
        else                 currentItem = 0;

        if (ImGui::Combo("Health State", &currentItem, stateItems, 6))
        {
            playerHp = static_cast<double>(currentItem);
        }
        ImGui::Text("Recover Timer: %.2f", recoverDelayTimer);
        ImGui::Text("Invincibility Timer: %.2f", invincibilityTimer);

        ImGui::Separator();
        ImGui::Text("Movement State:");

        ImGui::Checkbox("Is Jumping", &isJumping);
        ImGui::Checkbox("Is Sprinting", &isSprinting);
        ImGui::Checkbox("Face Right", &faceRight);

        ImGui::Text("Shift Hold Time: %.2f", shiftHoldTimer);
        ImGui::Text("Coyote Timer: %.2f", coyoteTimer);
        ImGui::Text("Jump Buffer: %.2f", jumpBufferTimer);

        ImGui::Separator();
        ImGui::Text("Dash State:");
        ImGui::Checkbox("Is Dashing", &dashComponent.isDashing);
        ImGui::Text("Dash Timer: %.2f", dashComponent.dashTimer);
        ImGui::Text("Dash Cooldown: %.2f", dashComponent.dashCooldownTimer);

        ImGui::Separator();
        if (shieldComponent)
        {
            shieldComponent->DrawImGui();
        }

        ImGui::TreePop();
    }
    ImGui::PopID();
}

void Player::ApplyLaserDamage(double damageAmount)
{
    if (healthState == HealthState::Dead)
    {
        return;
    }

    if (damageAmount <= 0.0)
    {
        return;
    }

    if (invincibilityTimer > 0.0)
    {
        return;
    }

    tookDamageThisFrame = true;
    recoverDelayTimer   = recoverDelayDuration;
    invincibilityTimer  = invincibilityDuration;
    
    playerHp = std::max(0.0, playerHp - damageAmount);

    if (playerHp <= 0.0)
    {
        playerHp    = 0.0;
        healthState = HealthState::Dead;
        Engine::GetLogger().LogEvent("Player died from laser damage.");
    }
}

void Player::UpdateHealthState(double dt)
{
    if (healthState == HealthState::Dead)
    {
        return;
    }

    // Regeneration Logic
    if (recoverDelayTimer > 0.0)
    {
        recoverDelayTimer = std::max(0.0, recoverDelayTimer - dt);
    }
    else if (playerHp < maxPlayerHp)
    {
        playerHp = std::min(maxPlayerHp, playerHp + 1.0);
        if (playerHp < maxPlayerHp)
        {
            recoverDelayTimer = recoverDelayDuration;
        }
    }

    // Update State based on HP
    int hpInt = static_cast<int>(playerHp + 0.001);
    if (hpInt >= 5)      healthState = HealthState::Full;
    else if (hpInt == 4) healthState = HealthState::Healthy;
    else if (hpInt == 3) healthState = HealthState::Hurt;
    else if (hpInt == 2) healthState = HealthState::Critical;
    else if (hpInt == 1) healthState = HealthState::NearDeath;
    else                 healthState = HealthState::Dead;

    tookDamageThisFrame = false;
}