#pragma once
#include "CS200/RGBA.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

class TargetStar : public CS230::GameObject
{
public:
    TargetStar(Math::vec2 position);

    void Draw([[maybe_unused]] const Math::TransformationMatrix& camera_matrix) override;

    // 레이저에 맞았을 때 호출될 함수
    void OnHit();

    // 별의 반지름 반환 (충돌 체크용)
    double GetRadius() const
    {
        return radius;
    }

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Target;
    }

    std::string TypeName() override
    {
        return "TargetStar";
    }

private:
    CS200::RGBA color;
    double      radius = 40.0;
};