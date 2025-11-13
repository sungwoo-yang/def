#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/Polygon.h"
#include "Engine/Vec2.hpp"
#include <vector>
#include "Engine/GameObjectTypes.hpp"

// 님의 엔진 네임스페이스
namespace CS230
{
    // SVG 경로를 그리기 위한 전용 GameObject
    class MapElement : public CS230::GameObject
    {
    public:
        // 생성자: 월드 좌표 위치와 로컬 폴리곤 데이터를 받음
        MapElement(Math::vec2 pos, Polygon polygon);

        // GameObject의 가상 함수 오버라이드
        void Draw(const Math::TransformationMatrix& camera_matrix) override;

        // TODO: source/Engine/GameObjectTypes.hpp 파일에 'MapElement' 같은 새 타입을 추가하세요.
        GameObjectTypes Type() override
        {
            return GameObjectTypes::Floor;
        } // 임시로 Floor 타입 사용

        std::string TypeName() override
        {
            return "MapElement";
        }

    private:
        Polygon local_polygon; // 이 객체의 로컬 (0,0 중심) 폴리곤
    };
}