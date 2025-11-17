#include "YellowLaser.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Physics/Reflection.hpp" // [!] 반사 엔진 사용
#include "Game/Player.hpp"
#include "Game/Shield.hpp"
#include <algorithm>
#include <cmath>

YellowLaser::YellowLaser(Math::vec2 startPos, Math::vec2 direction, Player* targetPlayer) : CS230::GameObject(startPos), startPosition(startPos), direction(direction.Normalize()), player(targetPlayer)
{
}

void YellowLaser::Update(double dt)
{
    // 1. 레이저 이동 (머리 전진)
    currentDistance += speed * dt;

    // 꼬리가 최대 사거리를 벗어나면 삭제
    double tailDistance = std::max(0.0, currentDistance - length);
    if (tailDistance > maxRange)
    {
        Destroy();
        return;
    }

    // 2. 반사 경로 계산 (매 프레임 갱신)
    fullPath.clear();
    std::vector<std::pair<Math::vec2, Math::vec2>> walls;

    // 쉴드가 '가드(Space)' 상태일 때만 반사벽으로 추가
    if (player != nullptr)
    {
        Shield* shield = player->GetShield();
        // 쉴드가 있고 + 가드 키(Space)를 누르고 있을 때
        if (shield && shield->IsGuardUp())
        {
            // 쉴드의 선분 정보를 가져와서 벽 목록에 추가
            auto segments = shield->GetSegments();
            walls.insert(walls.end(), segments.begin(), segments.end());
        }
    }

    // [Physics 엔진 사용] 경로 계산 (최대 2번 반사)
    fullPath = Physics::CalculateLaserPath(startPosition, direction, walls, 2);
}

void YellowLaser::Draw(const Math::TransformationMatrix& camera_matrix)
{
    auto& renderer = Engine::GetRenderer2D();

    // 3. 계산된 경로 위에 '뱀(Laser)' 그리기
    double headDist = currentDistance;
    double tailDist = std::max(0.0, currentDistance - length);

    if (headDist <= 0.0)
        return;

    double distanceTravelled = 0.0; // 경로를 따라간 누적 거리

    // 경로의 각 선분(Segment)을 순회하며 그릴 부분을 잘라냄
    for (const auto& segment : fullPath)
    {
        Math::vec2 p1 = segment.first;
        Math::vec2 p2 = segment.second;

        Math::vec2 segVec = p2 - p1;
        double     segLen = segVec.Length();

        // 현재 선분의 거리 범위: [start, end]
        double segStartDist = distanceTravelled;
        double segEndDist   = distanceTravelled + segLen;

        // 레이저(뱀)가 이 선분 구간에 걸쳐 있는지 확인
        // (선분의 끝이 꼬리보다 뒤에 있고, 선분의 시작이 머리보다 앞에 있어야 함)
        if (segEndDist >= tailDist && segStartDist <= headDist)
        {
            // 이 선분 안에서 실제로 그려야 할 로컬 시작/끝 지점 계산 (Clamping)
            // 로컬 좌표 = 전체 거리 - 현재 선분 시작 거리
            double drawStartLocal = std::max(0.0, tailDist - segStartDist);
            double drawEndLocal   = std::min(segLen, headDist - segStartDist);

            if (drawStartLocal < drawEndLocal)
            {
                Math::vec2 segDir = segVec.Normalize();
                Math::vec2 drawP1 = p1 + segDir * drawStartLocal;
                Math::vec2 drawP2 = p1 + segDir * drawEndLocal;

                // 노란색 레이저 그리기 (두께 10)
                renderer.DrawLine(camera_matrix * drawP1, camera_matrix * drawP2, 0xFFFF00FF, 10.0);
            }
        }

        distanceTravelled += segLen;

        // 레이저 머리보다 더 멀리 있는 경로는 계산할 필요 없음 (최적화)
        if (distanceTravelled > headDist)
            break;
    }
}