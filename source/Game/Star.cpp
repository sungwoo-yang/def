#include "Star.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Player.hpp"
#include "YellowLaser.hpp"

Star::Star(Math::vec2 position, Player* targetPlayer) : CS230::GameObject(position), player(targetPlayer), currentState(State::Idle), timer(0.0)
{
}

void Star::Update(double dt)
{
    if (player == nullptr)
        return;

    Math::vec2 myPos     = GetPosition();
    Math::vec2 playerPos = player->GetPosition();
    double     distance  = (playerPos - myPos).Length();

    switch (currentState)
    {
        case State::Idle:
            if (distance <= detectionRadius)
            {
                currentState = State::Warning;
                timer        = warningDuration;
                Engine::GetLogger().LogEvent("Star detected player! Warning started.");
            }
            break;

        case State::Warning:
            timer -= dt;
            if (timer <= 0.0)
            {
                // 경고 종료 -> 발사
                Math::vec2 dir = (playerPos - myPos).Normalize();

                // [수정] 생성자에 player 전달 (반사 로직을 위해)
                YellowLaser* laser = new YellowLaser(myPos, dir, player);

                Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(laser);
                Engine::GetLogger().LogEvent("Star fired Yellow Laser!");

                currentState = State::Cooldown;
                timer        = cooldownDuration;
            }
            break;

        case State::Cooldown:
            timer -= dt;
            if (timer <= 0.0)
            {
                currentState = State::Idle;
            }
            break;
    }

    CS230::GameObject::Update(dt);
}

void Star::Draw(const Math::TransformationMatrix& camera_matrix)
{
    auto& renderer = Engine::GetRenderer2D();

    // 1. 별 본체 (빨간색 원)
    Math::TransformationMatrix transform = camera_matrix * GetMatrix() * Math::ScaleMatrix({ 40.0, 40.0 });
    renderer.DrawCircle(transform, color);

    // 2. 감지 범위 표시 (디버그용, 얇은 회색 원)
    Math::TransformationMatrix rangeTransform = camera_matrix * Math::TranslationMatrix(GetPosition()) * Math::ScaleMatrix({ detectionRadius, detectionRadius });
    renderer.DrawCircle(rangeTransform, CS200::CLEAR, 0x808080FF, 1.0);

    // 3. 경고 상태일 때 보조선 표시
    if (currentState == State::Warning && player != nullptr)
    {
        // 별에서 플레이어까지 얇은 노란색 선 (색상 변경 없음)
        renderer.DrawLine(camera_matrix * GetPosition(), camera_matrix * player->GetPosition(), 0xFFFF0080, 2.0);
    }
}