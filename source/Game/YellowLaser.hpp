#pragma once

#include "CS200/RGBA.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include "Engine/Vec2.hpp"
#include <deque>
#include <vector>

class Player;
class TargetStar;

class YellowLaser : public CS230::GameObject
{
public:
    YellowLaser(Math::vec2 startPos, Math::vec2 direction, Player* player, const std::vector<TargetStar*>& targets);

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
    Math::vec2               headPosition;
    Math::vec2               velocity;
    Player*                  player;
    std::vector<TargetStar*> targets;
    std::deque<Math::vec2>   pathPoints;

    const double speed         = 600.0;
    const double maxBeamLength = 600.0;
    const double maxRange      = 4000.0;

    double bounceCooldown = 0.0;
    double currentLength  = 0.0;
};