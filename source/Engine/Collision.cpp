/**
 * \file
 * \author Sungwoo Yang (및 SATCollision 로직 포팅)
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */

#include "Engine/Collision.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Polygon.h" // 2단계에서 생성한 Polygon.h

/**
 * @brief SAT 충돌 검사를 위한 헬퍼 함수입니다.
 * 다각형의 정점들을 지정된 축(axis)에 투영(projection)하여 최소(min) 및 최대(max) 값을 찾습니다.
 * @param polygon 검사할 다각형
 * @param axis 투영할 축 (단위 벡터여야 함)
 * @param min 투영된 최소값 (결과 반환용)
 * @param max 투영된 최대값 (결과 반환용)
 */
void ProjectPolygon(const Polygon& polygon, const Math::vec2& axis, double& min, double& max)
{
    if (polygon.vertices.empty())
    {
        min = 0;
        max = 0;
        return;
    }

    // 1단계에서 Vec2.hpp에 추가한 dot() 함수를 사용합니다.
    double project_result = dot(polygon.vertices[0], axis);
    min                   = project_result;
    max                   = project_result;

    // Polygon.h에서 vertexCount 대신 .size()를 사용하도록 수정했습니다.
    for (size_t i = 1; i < polygon.vertices.size(); i++)
    {
        project_result = dot(polygon.vertices[i], axis);
        if (project_result < min)
            min = project_result;
        if (project_result > max)
            max = project_result;
    }
}

namespace CS230
{
    //--- RectCollision 구현 ---

    RectCollision::RectCollision(Math::irect bound, GameObject* obj) : boundary(bound), object(obj)
    {
    }

    Math::rect RectCollision::WorldBoundary()
    {
        // GameObject의 GetMatrix()를 사용하여 월드 좌표를 계산합니다.
        return { object->GetMatrix() * static_cast<Math::vec2>(boundary.point_1), object->GetMatrix() * static_cast<Math::vec2>(boundary.point_2) };
    }

    void RectCollision::Draw(const Math::TransformationMatrix& display_matrix)
    {
        auto&      renderer       = Engine::GetRenderer2D();
        Math::rect world_boundary = WorldBoundary();

        // 월드 좌표를 카메라(display_matrix)에 적용
        Math::vec2 bottom_left  = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Bottom() };
        Math::vec2 bottom_right = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Bottom() };
        Math::vec2 top_left     = display_matrix * Math::vec2{ world_boundary.Left(), world_boundary.Top() };
        Math::vec2 top_right    = display_matrix * Math::vec2{ world_boundary.Right(), world_boundary.Top() };

        // 님의 엔진 렌더러(IRenderer2D)를 사용해 그립니다.
        renderer.DrawLine(top_left, top_right, CS200::WHITE, 1.0);
        renderer.DrawLine(top_right, bottom_right, CS200::WHITE, 1.0);
        renderer.DrawLine(bottom_right, bottom_left, CS200::WHITE, 1.0); // (원본 버그 수정)
        renderer.DrawLine(bottom_left, top_left, CS200::WHITE, 1.0);     // (원본 버그 수정)
    }

    bool RectCollision::IsCollidingWith(GameObject* other_object)
    {
        Collision* other_collider = other_object->GetGOComponent<Collision>();

        if (other_collider == nullptr)
        {
            // --- ?? 수정된 부분 ?? ---
            Engine::GetLogger().LogDebug("No collision component found in other object");
            // --- ?? 수정 완료 ?? ---
            return false;
        }

        Math::rect rectangle_1 = WorldBoundary();

        // 1. Rect vs Rect (기존 로직)
        if (other_collider->Shape() == CollisionShape::Rect)
        {
            Math::rect rectangle_2 = static_cast<RectCollision*>(other_collider)->WorldBoundary();
            return (rectangle_1.Right() > rectangle_2.Left() && rectangle_1.Left() < rectangle_2.Right() && rectangle_1.Top() > rectangle_2.Bottom() && rectangle_1.Bottom() < rectangle_2.Top());
        }
        // 2. Rect vs Poly (SATCollision에 위임)
        else if (other_collider->Shape() == CollisionShape::Poly)
        {
            // (A vs B) 충돌은 (B vs A) 충돌과 같습니다.
            // SATCollision의 IsCollidingWith가 Rect를 처리하도록 위임합니다.
            return other_collider->IsCollidingWith(this->object);
        }
        // 3. Rect vs Circle (미구현)
        else if (other_collider->Shape() == CollisionShape::Circle)
        {
            // --- ?? 수정된 부분 ?? ---
            Engine::GetLogger().LogDebug("Rect vs Circle collision not yet implemented.");
            // --- ?? 수정 완료 ?? ---
            return false;
        }

        Engine::GetLogger().LogError("Rect vs unsupported type");
        return false;
    }

    bool RectCollision::IsCollidingWith(Math::vec2 point)
    {
        Math::rect rect = WorldBoundary();
        return (point.x >= rect.Left() && point.x < rect.Right() && point.y >= rect.Bottom() && point.y <= rect.Top());
    }

    //--- CircleCollision 구현 ---

    CircleCollision::CircleCollision(double rad, GameObject* obj) : radius(rad), object(obj)
    {
    }

    void CircleCollision::Draw(const Math::TransformationMatrix& display_matrix)
    {
        auto& renderer = Engine::GetRenderer2D();

        // 오브젝트의 변환(위치, 크기)과 카메라(display_matrix)를 모두 적용합니다.
        Math::TransformationMatrix transform = display_matrix * Math::TranslationMatrix(object->GetPosition()) * Math::ScaleMatrix(Math::vec2{ GetRadius(), GetRadius() });

        renderer.DrawCircle(transform, CS200::CLEAR, CS200::WHITE, 1.0);
    }

    double CircleCollision::GetRadius()
    {
        Math::vec2 scale = object->GetScale();
        // 오브젝트의 스케일을 반영합니다.
        return scale.x > scale.y ? radius * scale.x : radius * scale.y;
    }

    bool CircleCollision::IsCollidingWith(GameObject* other_object)
    {
        Collision* other_collider = other_object->GetGOComponent<Collision>();
        if (other_collider == nullptr)
        {
            // --- ?? 수정된 부분 ?? ---
            Engine::GetLogger().LogDebug("No collision component found in other object");
            // --- ?? 수정 완료 ?? ---
            return false;
        }

        // 1. Circle vs Circle (기존 로직)
        if (other_collider->Shape() == CollisionShape::Circle)
        {
            CircleCollision* other       = dynamic_cast<CircleCollision*>(other_collider);
            Math::vec2       A           = object->GetPosition();
            Math::vec2       B           = other->object->GetPosition();
            double           dx          = A.x - B.x;
            double           dy          = A.y - B.y;
            double           distance_sq = dx * dx + dy * dy;
            double           sum_radius  = GetRadius() + other->GetRadius();
            return distance_sq <= sum_radius * sum_radius;
        }
        else
        {
            // --- ?? 수정된 부분 ?? ---
            Engine::GetLogger().LogDebug("Circle vs (Rect/Poly) collision not yet implemented.");
            // --- ?? 수정 완료 ?? ---
            return false;
        }
    }

    bool CircleCollision::IsCollidingWith(Math::vec2 point)
    {
        Math::vec2 center       = object->GetPosition();
        double     r            = GetRadius();
        Math::vec2 diff         = point - center;
        double     dist_squared = diff.x * diff.x + diff.y * diff.y;
        return dist_squared <= r * r;
    }

    //--- SATCollision 구현 ---

    SATCollision::SATCollision(Polygon bound, GameObject* obj) : boundary(bound), object(obj)
    {
        // Polygon.h에서 .size()를 사용하므로 vertexCount를 보장해줍니다.
        boundary.vertexCount = static_cast<int>(boundary.vertices.size());
    }

    void SATCollision::Draw(const Math::TransformationMatrix& display_matrix)
    {
        auto&   renderer       = Engine::GetRenderer2D();
        Polygon world_boundary = WorldBoundary(); // 현재 월드 좌표 폴리곤 계산

        if (world_boundary.vertexCount < 2)
            return;

        // 폴리곤의 모든 모서리를 그립니다.
        for (int i = 0; i < world_boundary.vertexCount; ++i)
        {
            Math::vec2 p1 = world_boundary.vertices[i];
            Math::vec2 p2 = world_boundary.vertices[(i + 1) % world_boundary.vertexCount];

            // 카메라가 적용된 최종 매트릭스로 그립니다.
            renderer.DrawLine(display_matrix * p1, display_matrix * p2, CS200::WHITE, 1.0);
        }
    }

    Polygon SATCollision::WorldBoundary()
    {
        Polygon world_poly;
        world_poly.vertexCount = boundary.vertexCount;
        world_poly.vertices.reserve(boundary.vertexCount);

        // GameObject의 현재 변환 행렬을 가져옵니다.
        const Math::TransformationMatrix& matrix = object->GetMatrix();

        // 로컬 좌표 정점들을 월드 좌표로 변환합니다.
        for (const auto& v : boundary.vertices)
        {
            world_poly.vertices.push_back(matrix * v);
        }
        return world_poly;
    }

    bool SATCollision::IsCollidingWith(Math::vec2 point)
    {
        Polygon poly_1 = WorldBoundary();
        if (poly_1.vertexCount == 0)
            return false;

        // 이 다각형의 모든 축(모서리의 법선)을 검사합니다.
        for (int i = 0; i < poly_1.vertexCount; i++)
        {
            Math::vec2 edge = poly_1.vertices[(i + 1) % poly_1.vertexCount] - poly_1.vertices[i];
            Math::vec2 axis = GetPerpendicular(edge).Normalize(); // 1단계에서 추가

            double minA, maxA;
            ProjectPolygon(poly_1, axis, minA, maxA);
            double projection = dot(point, axis); // 1단계에서 추가

            // 축에 투영된 점이 다각형의 투영 범위 밖에 있으면 충돌하지 않음
            if (projection < minA || projection > maxA)
            {
                return false; // 분리 축 발견!
            }
        }
        return true; // 모든 축에서 겹쳤으므로 충돌
    }

    bool SATCollision::IsCollidingWith(GameObject* other_object)
    {
        Collision* other_collider = other_object->GetGOComponent<Collision>();
        if (other_collider == nullptr)
            return false;

        Polygon poly_1 = WorldBoundary(); // 나의 월드 폴리곤
        Polygon poly_2;                   // 상대방의 월드 폴리곤

        // 상대방의 모양(Shape)에 따라 poly_2를 설정합니다.
        if (other_collider->Shape() == CollisionShape::Poly)
        {
            poly_2 = static_cast<SATCollision*>(other_collider)->WorldBoundary();
        }
        else if (other_collider->Shape() == CollisionShape::Rect)
        {
            Math::rect rect = static_cast<RectCollision*>(other_collider)->WorldBoundary();
            poly_2.vertices = {
                {  rect.Left(), rect.Bottom() },
                { rect.Right(), rect.Bottom() },
                { rect.Right(),    rect.Top() },
                {  rect.Left(),    rect.Top() }
            };
            poly_2.vertexCount = 4;
        }
        else if (other_collider->Shape() == CollisionShape::Circle)
        {
            // --- ?? 수정된 부분 ?? ---
            Engine::GetLogger().LogDebug("SATCollision vs Circle-Collision은 아직 지원되지 않습니다.");
            // --- ?? 수정 완료 ?? ---
            return false;
        }

        if (poly_1.vertexCount == 0 || poly_2.vertexCount == 0)
            return false;

        // --- SAT (분리 축 이론) 알고리즘 ---

        // 1. 나의(poly_1) 모든 축 검사
        for (int i = 0; i < poly_1.vertexCount; i++)
        {
            Math::vec2 edge = poly_1.vertices[(i + 1) % poly_1.vertexCount] - poly_1.vertices[i];
            Math::vec2 axis = GetPerpendicular(edge).Normalize();

            double minA, maxA, minB, maxB;
            ProjectPolygon(poly_1, axis, minA, maxA);
            ProjectPolygon(poly_2, axis, minB, maxB);

            // 두 다각형의 투영이 겹치지 않으면, 분리된 것이므로 충돌하지 않음
            if (maxA < minB || maxB < minA)
                return false;
        }

        // 2. 상대방의(poly_2) 모든 축 검사
        for (int i = 0; i < poly_2.vertexCount; i++)
        {
            Math::vec2 edge = poly_2.vertices[(i + 1) % poly_2.vertexCount] - poly_2.vertices[i];
            Math::vec2 axis = GetPerpendicular(edge).Normalize();

            double minA, maxA, minB, maxB;
            ProjectPolygon(poly_1, axis, minA, maxA);
            ProjectPolygon(poly_2, axis, minB, maxB);

            if (maxA < minB || maxB < minA)
                return false;
        }

        // 모든 축에서 겹쳤으므로 충돌
        return true;
    }

} // namespace CS230