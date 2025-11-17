#pragma once
#include "CS200/RGBA.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

class Player; // 전방 선언
class TargetStar;

class Star : public CS230::GameObject
{
public:
    Star(Math::vec2 position, Player* targetPlayer, TargetStar* destStar);

    void Update(double dt) override;
    void Draw([[maybe_unused]] const Math::TransformationMatrix& camera_matrix) override;

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Star;
    }

    std::string TypeName() override
    {
        return "Star";
    }

private:
    enum class State
    {
        Idle,    // 감지 대기
        Warning, // 2초간 조준 (보조선)
        Cooldown // 3초간 대기
    };

    Player*     player;
    TargetStar* target;
    State       currentState;
    double      timer;

    const double detectionRadius  = 1000.0;
    const double warningDuration  = 3.0;
    const double cooldownDuration = 5.0;

    CS200::RGBA color = 0xFF0000FF; // 빨간색
};