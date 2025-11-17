// source/Game/YellowLaser.cpp

#include "YellowLaser.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Physics/Reflection.hpp"
#include "Player.hpp"
#include "Shield.hpp"
#include "TargetStar.hpp"
#include <algorithm>
#include <cmath>

double DistToSegmentSquared(Math::vec2 p, Math::vec2 v, Math::vec2 w)
{
    double l2 = (w - v).Dot(w - v);
    if (l2 == 0.0)
        return (p - v).Dot(p - v);
    double t              = ((p - v).Dot(w - v)) / l2;
    t                     = std::max(0.0, std::min(1.0, t));
    Math::vec2 projection = v + (w - v) * t;
    return (p - projection).Dot(p - projection);
}

YellowLaser::YellowLaser(Math::vec2 startPos, Math::vec2 direction, Player* player, TargetStar* target) : CS230::GameObject(startPos), headPosition(startPos), player(player), target(target)
{
    velocity = direction.Normalize() * speed;
    pathPoints.push_back(startPos);
}

void YellowLaser::Update(double dt)
{
    // 1. 머리 이동 (이번 프레임 예상 위치)
    Math::vec2 nextPos = headPosition + velocity * dt;

    // 2. 쉴드 충돌 체크 (RayCast)
    bool bounced = false;
    if (player != nullptr)
    {
        Shield* shield = player->GetShield();
        // 가드 중일 때만 충돌 체크
        if (shield && shield->IsGuardUp())
        {
            auto segments = shield->GetSegments();
            if (!segments.empty())
            {
                Math::vec2 segStart = segments[0].first;
                Math::vec2 segEnd   = segments[0].second;

                Math::vec2 intersection;
                double     t;

                Math::vec2 rayDir   = velocity.Normalize();
                double     moveDist = (nextPos - headPosition).Length();

                if (Physics::RaySegmentIntersection(headPosition, rayDir, segStart, segEnd, intersection, t))
                {
                    if (t >= 0 && t <= moveDist)
                    {
                        // 충돌 발생!
                        // 경로에 충돌 지점 추가
                        pathPoints.push_back(intersection);

                        // 반사 벡터 계산
                        Math::vec2 wallVec = segEnd - segStart;
                        Math::vec2 normal  = Math::vec2{ -wallVec.y, wallVec.x }.Normalize();
                        if (Math::dot(rayDir, normal) > 0)
                            normal = -normal;

                        Math::vec2 reflectDir = Physics::CalculateReflection(rayDir, normal);
                        velocity              = reflectDir * speed;

                        // 헤드 위치 갱신 (반사된 만큼 남은 거리 이동)
                        double remainingDist = moveDist - t;
                        headPosition         = intersection + velocity.Normalize() * remainingDist;

                        bounced = true;
                    }
                }
            }
        }
    }

    if (!bounced)
    {
        // 충돌 없으면 그냥 전진
        headPosition = nextPos;
    }

    // 3. 꼬리 자르기 (길이 제한)
    // [수정된 로직] 반복문 조건을 true로 변경하고 내부에서 break 처리
    while (true)
    {
        if (pathPoints.empty())
            break;

        // 현재 총 길이 계산
        double totalLenWithHead = 0;
        // 저장된 경로 점들 간의 거리
        for (size_t i = 0; i < pathPoints.size() - 1; ++i)
        {
            totalLenWithHead += (pathPoints[i + 1] - pathPoints[i]).Length();
        }
        // 마지막 점 ~ 현재 헤드 거리
        totalLenWithHead += (headPosition - pathPoints.back()).Length();

        // 길이가 적당하면 종료
        if (totalLenWithHead <= maxBeamLength)
        {
            break;
        }

        // 초과된 길이만큼 꼬리를 당겨야 함
        double overflow = totalLenWithHead - maxBeamLength;

        if (pathPoints.size() > 1)
        {
            // 세그먼트가 2개 이상 (Start -> Ref1 -> ... -> Head)
            double firstSegLen = (pathPoints[1] - pathPoints[0]).Length();

            if (overflow >= firstSegLen)
            {
                // 첫 번째 세그먼트가 완전히 사라짐 -> 점 제거하고 다시 루프
                pathPoints.pop_front();
            }
            else
            {
                // 첫 번째 세그먼트 일부만 사라짐 -> 시작점을 다음 점 방향으로 이동
                Math::vec2 dir = (pathPoints[1] - pathPoints[0]).Normalize();
                pathPoints[0] += dir * overflow;
                break; // 조정 완료
            }
        }
        else // pathPoints.size() == 1
        {
            // [중요] 반사점이 없고(또는 다 사라지고) 점이 하나만 남았을 때
            // (LastPoint -> Head)
            // 이 점을 Head 방향으로 이동시켜서 꼬리를 따라가게 함
            Math::vec2 dir = (headPosition - pathPoints[0]).Normalize();
            pathPoints[0] += dir * overflow;
            break; // 조정 완료
        }
    }

    if (target != nullptr)
    {
        double     targetRadiusSq = target->GetRadius() * target->GetRadius();
        Math::vec2 targetPos      = target->GetPosition();

        // 현재 활성화된 레이저 세그먼트들과 충돌 검사
        // 1. pathPoints 사이의 선분들
        for (size_t i = 0; i < pathPoints.size() - 1; ++i)
        {
            if (DistToSegmentSquared(targetPos, pathPoints[i], pathPoints[i + 1]) <= targetRadiusSq)
            {
                target->OnHit();
                break;
            }
        }
        // 2. 마지막 pathPoint ~ headPosition 선분
        if (!pathPoints.empty())
        {
            if (DistToSegmentSquared(targetPos, pathPoints.back(), headPosition) <= targetRadiusSq)
            {
                target->OnHit();
            }
        }
    }

    // 4. 화면 밖으로 나가서 삭제 (꼬리가 너무 멀리 감)
    // pathPoints가 비었거나, 하나 남았는데 헤드랑 너무 멀 때
    if (pathPoints.empty())
    {
        Destroy();
    }
    else if (pathPoints.size() == 1)
    {
        // 시작점이 이미 4000 이상 나갔다면 삭제 (안전장치)
        // 실제로는 위 로직에 의해 maxBeamLength가 유지되므로,
        // pathPoints[0]의 좌표 자체가 맵 밖으로 나갔는지 확인하는 게 좋음
        if (pathPoints[0].x > maxRange + 5000 || pathPoints[0].x < -1000 || pathPoints[0].y > maxRange + 2000 || pathPoints[0].y < -1000)
        {
            Destroy();
        }
    }
}

// Draw 함수는 변경 없음 (그대로 두셔도 됩니다)
void YellowLaser::Draw(const Math::TransformationMatrix& /*camera_matrix*/)
{
    auto& renderer = Engine::GetRenderer2D();

    if (pathPoints.empty())
        return;

    // 1. 저장된 꺾임점들 그리기
    for (size_t i = 0; i < pathPoints.size() - 1; ++i)
    {
        renderer.DrawLine(pathPoints[i], pathPoints[i + 1], 0xFFFF00FF, 10.0);
    }

    // 2. 마지막 꺾임점 ~ 현재 머리 위치 그리기
    renderer.DrawLine(pathPoints.back(), headPosition, 0xFFFF00FF, 10.0);
}