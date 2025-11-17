#include "Star.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Player.hpp"
#include "RedLaser.hpp"
#include "Shield.hpp"
#include "YellowLaser.hpp"

Star::Star(Math::vec2 position, Player* targetPlayer, const std::vector<TargetStar*>& destStars, StarType type)
    : CS230::GameObject(position), player(targetPlayer), targets(destStars), starType(type), currentState(State::Idle), timer(0.0)
{
    // 타입에 따른 색상 설정
    if (starType == StarType::Red)
        color = 0xFF0000FF; // 빨강
    else
        color = 0xFFFF00FF; // 노랑 (또는 기존 색)
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
            if (starType == StarType::Red)
            {
                Shield* shield = player->GetShield();
                if (shield)
                {
                    if (timer <= parryWindowTime)
                        shield->SetParryWindowActive(true);
                    else
                        shield->SetParryWindowActive(false);
                }
            }

            timer -= dt;
            if (timer <= 0.0)
            {
                Math::vec2 dir = (player->GetPosition() - GetPosition()).Normalize();

                if (starType == StarType::Yellow)
                {
                    YellowLaser* laser = new YellowLaser(GetPosition(), dir, player, targets);
                    Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(laser);
                }
                else // Red
                {
                    if (player->GetShield())
                        player->GetShield()->SetParryWindowActive(false);

                    // [수정] targets 벡터 전달
                    RedLaser* laser = new RedLaser(GetPosition(), dir, player, targets);
                    Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->Add(laser);
                }

                Engine::GetLogger().LogEvent("Star fired Laser!");
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

void Star::Draw([[maybe_unused]] const Math::TransformationMatrix& camera_matrix)
{
    auto& renderer = Engine::GetRenderer2D();

    // 1. 별 본체
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix({ 40.0, 40.0 });
    renderer.DrawCircle(transform, color);

    // 2. 감지 범위
    Math::TransformationMatrix rangeTransform = Math::TranslationMatrix(GetPosition()) * Math::ScaleMatrix({ detectionRadius * 2.0, detectionRadius * 2.0 });
    renderer.DrawCircle(rangeTransform, CS200::CLEAR, 0x808080FF, 1.0);

    // 3. 보조선 그리기
    if (currentState == State::Warning && player != nullptr)
    {
        CS200::RGBA lineColor;

        // [Red Star] 패링 타이밍(0.5초 이하)에 색상 변경 (노랑 -> 시안)
        if (starType == StarType::Red)
        {
            if (timer <= 0.5)
            {
                lineColor = 0x00FFFF80; // 패링 신호 (Cyan)
            }
            else
            {
                lineColor = 0xFF000080; // 평소엔 빨간색(또는 노란색)으로 경고
            }
        }
        else
        {
            lineColor = 0xFFFF0080;
        }
        // 2. 반사 경로 계산 (Red, Yellow 공통)
        std::vector<std::pair<Math::vec2, Math::vec2>> walls;
        Shield*                                        shield = player->GetShield();

        if (shield)
        {
            // Red: 항상 반사 경로 표시 (조준용)
            // Yellow: 가드 중일 때만 표시
            if (starType == StarType::Red || (starType == StarType::Yellow && shield->IsGuardUp()))
            {
                auto segments = shield->GetSegments();
                if (!segments.empty())
                    walls.push_back(segments[0]);
            }
        }

        Math::vec2 dir = (player->GetPosition() - GetPosition()).Normalize();

        // 물리 엔진으로 경로 계산
        auto path = Physics::CalculateLaserPath(GetPosition(), dir, walls, 2);

        // 경로 그리기
        for (const auto& seg : path)
        {
            renderer.DrawLine(seg.first, seg.second, lineColor, 2.0);
        }
    }
}