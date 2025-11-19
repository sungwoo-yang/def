#include "Star.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Game/Shield.hpp"
#include "Player.hpp"
#include "RedLaser.hpp"
#include "YellowLaser.hpp"

Star::Star(Math::vec2 position, Player* targetPlayer, const std::vector<TargetStar*>& destStars, StarType type)
    : CS230::GameObject(position), player(targetPlayer), targets(destStars), starType(type), currentState(State::Idle), timer(0.0)
{
    if (starType == StarType::Red)
        color = 0xFF0000FF;
    else
        color = 0xFFFF00FF;
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
                else
                {
                    if (player->GetShield())
                        player->GetShield()->SetParryWindowActive(false);

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

    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix({ 40.0, 40.0 });
    renderer.DrawCircle(transform, color);

    Math::TransformationMatrix rangeTransform = Math::TranslationMatrix(GetPosition()) * Math::ScaleMatrix({ detectionRadius * 2.0, detectionRadius * 2.0 });
    renderer.DrawCircle(rangeTransform, CS200::CLEAR, 0x808080FF, 1.0);

    if (currentState == State::Warning && player != nullptr)
    {
        CS200::RGBA lineColor;

        if (starType == StarType::Red)
        {
            lineColor = (timer <= 0.5) ? 0x00FFFF80 : 0xFF000080;
        }
        else
        {
            lineColor = 0xFFFF0080;
        }

        std::vector<std::pair<Math::vec2, Math::vec2>> walls;
        Shield*                                        shield = player->GetShield();

        if (shield && shield->IsGuardUp())
        {
            auto segments = shield->GetSegments();
            if (!segments.empty())
                walls.push_back(segments[0]);
        }

        Math::vec2 dir = (player->GetPosition() - GetPosition()).Normalize();

        auto path = Physics::CalculateLaserPath(GetPosition(), dir, walls, 2);

        for (const auto& seg : path)
        {
            renderer.DrawLine(seg.first, seg.second, lineColor, 2.0);
        }
    }
}