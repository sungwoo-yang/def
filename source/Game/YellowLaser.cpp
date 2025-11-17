#include "YellowLaser.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Player.hpp"
#include "Shield.hpp"
#include "TargetStar.hpp"
#include <algorithm>
#include <cmath>

// [필수] 거리 계산 함수
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

YellowLaser::YellowLaser(Math::vec2 startPos, Math::vec2 direction, Player* player, const std::vector<TargetStar*>& targets)
    : CS230::GameObject(startPos), headPosition(startPos), player(player), targets(targets)
{
    velocity = direction.Normalize() * speed;
    pathPoints.push_back(startPos);
}

void YellowLaser::Update(double dt)
{
    if (bounceCooldown > 0.0)
        bounceCooldown -= dt;
    Math::vec2 nextPos = headPosition + velocity * dt;
    bool       bounced = false;

    if (player != nullptr && bounceCooldown <= 0.0)
    {
        Shield* shield = player->GetShield();
        if (shield && shield->IsGuardUp())
        {
            auto segments = shield->GetSegments();
            for (const auto& seg : segments)
            {
                Math::vec2 segStart = seg.first;
                Math::vec2 segEnd   = seg.second;
                Math::vec2 intersection;
                double     t;
                Math::vec2 rayDir   = velocity.Normalize();
                double     moveDist = (nextPos - headPosition).Length();

                if (Physics::RaySegmentIntersection(headPosition, rayDir, segStart, segEnd, intersection, t))
                {
                    if (t >= 0 && t <= moveDist)
                    {
                        pathPoints.push_back(intersection);
                        Math::vec2 wallVec = segEnd - segStart;
                        Math::vec2 normal  = Math::vec2{ -wallVec.y, wallVec.x }.Normalize();
                        if (Math::dot(rayDir, normal) > 0)
                            normal = -normal;
                        Math::vec2 reflectDir = Physics::CalculateReflection(rayDir, normal);
                        velocity              = reflectDir * speed;
                        double remainingDist  = moveDist - t;
                        headPosition          = intersection + velocity.Normalize() * (remainingDist + 0.1);
                        bounced               = true;
                        bounceCooldown        = 0.1;
                        break;
                    }
                }
            }
        }
    }
    if (!bounced)
        headPosition = nextPos;

    while (true)
    {
        if (pathPoints.empty())
            break;
        double totalLen = 0;
        for (size_t i = 0; i < pathPoints.size() - 1; ++i)
            totalLen += (pathPoints[i + 1] - pathPoints[i]).Length();
        totalLen += (headPosition - pathPoints.back()).Length();
        if (totalLen <= maxBeamLength)
            break;
        double overflow = totalLen - maxBeamLength;
        if (pathPoints.size() > 1)
        {
            double firstSegLen = (pathPoints[1] - pathPoints[0]).Length();
            if (overflow >= firstSegLen)
                pathPoints.pop_front();
            else
            {
                Math::vec2 dir = (pathPoints[1] - pathPoints[0]).Normalize();
                pathPoints[0] += dir * overflow;
                break;
            }
        }
        else
        {
            Math::vec2 dir = (headPosition - pathPoints[0]).Normalize();
            pathPoints[0] += dir * overflow;
            break;
        }
    }

    // 타겟 충돌 체크
    for (TargetStar* target : targets)
    {
        if (target == nullptr)
            continue;
        double r2 = target->GetRadius() * target->GetRadius();

        bool hit = false;
        for (size_t i = 0; i < pathPoints.size() - 1; ++i)
        {
            if (DistToSegmentSquared(target->GetPosition(), pathPoints[i], pathPoints[i + 1]) <= r2)
            {
                hit = true;
                break;
            }
        }
        if (!hit && !pathPoints.empty())
        {
            if (DistToSegmentSquared(target->GetPosition(), pathPoints.back(), headPosition) <= r2)
                hit = true;
        }

        if (hit)
            target->OnHit();
    }

    if (pathPoints.empty())
    {
        Destroy();
    }
    else if (pathPoints.size() == 1)
    {
        if (pathPoints[0].x > maxRange + 5000 || pathPoints[0].x < -1000 || pathPoints[0].y > maxRange + 2000 || pathPoints[0].y < -1000)
        {
            Destroy();
        }
    }
}

void YellowLaser::Draw(const Math::TransformationMatrix& /*camera_matrix*/)
{
    auto& renderer = Engine::GetRenderer2D();
    if (pathPoints.empty())
        return;
    for (size_t i = 0; i < pathPoints.size() - 1; ++i)
        renderer.DrawLine(pathPoints[i], pathPoints[i + 1], 0xFFFF00FF, 10.0);
    renderer.DrawLine(pathPoints.back(), headPosition, 0xFFFF00FF, 10.0);
}