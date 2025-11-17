#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include "Engine/Vec2.hpp"
#include <deque>

class Player;
class TargetStar; // [추가] 전방 선언

class YellowLaser : public CS230::GameObject
{
public:
    // [수정] TargetStar* 인자 추가
    YellowLaser(Math::vec2 startPos, Math::vec2 direction, Player* player, TargetStar* target);

    void Update(double dt) override;
    void Draw(const Math::TransformationMatrix& camera_matrix) override;

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Laser;
    }

    std::string TypeName() override
    {
        return "YellowLaser";
    }

private:
    Math::vec2  headPosition;
    Math::vec2  velocity;
    Player*     player;
    TargetStar* target; // [추가] 목표 별 참조

    std::deque<Math::vec2> pathPoints;

    const double speed         = 600.0;
    const double maxBeamLength = 600.0;
    const double maxRange      = 4000.0;

    double currentLength = 0.0;
};