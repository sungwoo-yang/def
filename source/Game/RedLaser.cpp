#include "RedLaser.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Player.hpp"
#include "Shield.hpp"
#include "TargetStar.hpp"
#include <algorithm> // std::max, std::min

// 거리 계산 헬퍼 함수 (YellowLaser와 동일)
static double DistToSegmentSquared(Math::vec2 p, Math::vec2 v, Math::vec2 w)
{
    double l2 = (w - v).Dot(w - v);
    if (l2 == 0.0)
        return (p - v).Dot(p - v);
    double t              = ((p - v).Dot(w - v)) / l2;
    t                     = std::max(0.0, std::min(1.0, t));
    Math::vec2 projection = v + (w - v) * t;
    return (p - projection).Dot(p - projection);
}

RedLaser::RedLaser(Math::vec2 startPos, Math::vec2 direction, Player* player, const std::vector<TargetStar*>& targets)
    : CS230::GameObject(startPos), start(startPos), dir(direction.Normalize()), player(player), targets(targets)
{
}

void RedLaser::Update(double dt)
{
    lifeTime -= dt;
    if (lifeTime <= 0.0)
    {
        Destroy();
        return;
    }

    if (!isCalculated)
    {
        isCalculated = true;

        std::vector<std::pair<Math::vec2, Math::vec2>> walls;
        bool                                           parried = false;

        // 1. 패링 체크: 성공 시 쉴드를 벽으로 추가
        if (player != nullptr)
        {
            Shield* shield = player->GetShield();
            if (shield && shield->ConsumeParryState())
            {
                auto segments = shield->GetSegments();
                if (!segments.empty())
                {
                    walls.push_back(segments[0]);
                    parried = true;
                }
            }
        }

        // 2. 물리 엔진으로 경로 계산 (보조선과 100% 일치)
        // walls가 비어있으면 직선, 있으면 반사됨
        auto path = Physics::CalculateLaserPath(start, dir, walls, 2);

        // 3. 경로 순회하며 충돌 체크 (타겟 및 플레이어)
        bool hitSomething = false;

        for (const auto& seg : path)
        {
            Math::vec2  p1        = seg.first;
            Math::vec2  p2        = seg.second;
            CS200::RGBA beamColor = 0xFF0000FF; // 기본 빨강

            // (1) 타겟 별 충돌 체크
            for (TargetStar* target : targets)
            {
                if (target == nullptr)
                    continue;
                double r2 = target->GetRadius() * target->GetRadius();

                if (DistToSegmentSquared(target->GetPosition(), p1, p2) <= r2)
                {
                    target->OnHit(); // 타겟 색상 변경

                    // 시각적으로 타겟까지만 그림
                    // (정확한 교차점 계산은 생략하고 p2까지 그리거나, 필요 시 보정)
                    beams.push_back({ p1, target->GetPosition(), beamColor });
                    hitSomething = true;
                    Engine::GetLogger().LogEvent("Red Laser Hit Target!");
                    break;
                }
            }
            if (hitSomething)
                break;

            // (2) 플레이어 충돌 체크 (패링 실패 시에만)
            if (!parried && player != nullptr)
            {
                // 간단히 플레이어 중심과의 거리로 체크 (반경 40 정도 가정)
                double playerR2 = 40.0 * 40.0;
                if (DistToSegmentSquared(player->GetPosition(), p1, p2) <= playerR2)
                {
                    player->ResetState(); // 사망/리스폰
                    beams.push_back({ p1, player->GetPosition(), beamColor });
                    hitSomething = true;
                    Engine::GetLogger().LogEvent("Player Hit by Red Laser!");
                    break;
                }
            }
            if (hitSomething)
                break;

            // 아무것도 안 맞았으면 전체 세그먼트 추가
            beams.push_back({ p1, p2, beamColor });
        }
    }
}

void RedLaser::Draw(const Math::TransformationMatrix& /*camera_matrix*/)
{
    auto& renderer = Engine::GetRenderer2D();
    for (const auto& beam : beams)
    {
        renderer.DrawLine(beam.p1, beam.p2, beam.color, 15.0);
    }
}