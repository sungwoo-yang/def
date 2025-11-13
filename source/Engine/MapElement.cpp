#include "Engine/MapElement.h"
#include "CS200/IRenderer2D.hpp" // 렌더러 인터페이스
#include "CS200/RGBA.hpp"        // 색상
#include "Engine/Engine.hpp"     // 렌더러 접근
#include "Engine/Collision.hpp"

namespace CS230
{
    MapElement::MapElement(Math::vec2 pos, Polygon polygon)
        : CS230::GameObject(pos), // 부모 생성자 (위치 설정)
          local_polygon(std::move(polygon))
    {
        local_polygon.vertexCount = static_cast<int>(local_polygon.vertices.size());

        AddGOComponent(new SATCollision(local_polygon, this));
    }

    void MapElement::Draw(const Math::TransformationMatrix& camera_matrix)
    {
        // 1. 렌더러 가져오기
        CS200::IRenderer2D& renderer = Engine::GetRenderer2D();

        // 2. 이 객체의 Model-to-World 매트릭스 가져오기
        const Math::TransformationMatrix& model_matrix = GetMatrix();

        if (local_polygon.vertexCount < 2)
            return;

        // 3. 요청하신 'DrawLine(transform, start, end, ...)' 오버로드를 사용합니다.
        // 렌더러는 BeginScene에서 받은 카메라 매트릭스와
        // 이 model_matrix를 내부에서 조합하여 최종 좌표를 계산합니다.
        for (int i = 0; i < local_polygon.vertexCount; ++i)
        {
            Math::vec2 p1 = local_polygon.vertices[i];
            Math::vec2 p2 = local_polygon.vertices[(i + 1) % local_polygon.vertexCount];

            // 렌더러에 그리기 요청
            renderer.DrawLine(model_matrix, p1, p2, CS200::WHITE, 1.0);
        }

        // 4. (필수) 부모의 Draw()를 호출하여 Collision 등 다른 컴포넌트 그리기
        CS230::GameObject::Draw(camera_matrix);
    }
}