#include "Star.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Player.hpp"
#include "Shield.hpp"
#include "YellowLaser.hpp"

Star::Star(Math::vec2 position, Player* targetPlayer, TargetStar* destStar) : CS230::GameObject(position), player(targetPlayer), target(destStar), currentState(State::Idle), timer(0.0)
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
                Math::vec2 dir = (player->GetPosition() - GetPosition()).Normalize();

                // [수정] YellowLaser 생성 시 target 전달
                YellowLaser* laser = new YellowLaser(GetPosition(), dir, player, target);

                Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(laser);
                // ...
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

void Star::Draw([[maybe_unused]] const Math::TransformationMatrix& camera_matrix)
{
    auto& renderer = Engine::GetRenderer2D();

    // 1. 별 본체
    // 월드 좌표 기준 Transform 생성
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix({ 40.0, 40.0 });
    renderer.DrawCircle(transform, color);

    // 2. 감지 범위 표시
    Math::TransformationMatrix rangeTransform = Math::TranslationMatrix(GetPosition()) * Math::ScaleMatrix({ detectionRadius, detectionRadius });
    renderer.DrawCircle(rangeTransform, CS200::CLEAR, 0x808080FF, 1.0);

    // 3. 보조선
    if (currentState == State::Warning && player != nullptr)
    {
        std::vector<std::pair<Math::vec2, Math::vec2>> walls;
        Shield*                                        shield = player->GetShield();

        // 가드 중이면 벽 추가
        if (shield && shield->IsGuardUp())
        {
            walls.push_back(shield->GetSegments()[0]);
        }

        Math::vec2 dir = (player->GetPosition() - GetPosition()).Normalize();

        // 경로 계산 (최대 2회 반사)
        auto path = Physics::CalculateLaserPath(GetPosition(), dir, walls, 2);

        for (const auto& seg : path)
        {
            // 노란색 점선 느낌 (반투명)
            renderer.DrawLine(seg.first, seg.second, 0xFFFF0080, 2.0);
        }
    }
}