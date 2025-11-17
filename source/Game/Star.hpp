#pragma once
#include "CS200/RGBA.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

class Player; // 전방 선언

class Star : public CS230::GameObject
{
public:
    Star(Math::vec2 position, Player* targetPlayer);

    void Update(double dt) override;
    void Draw(const Math::TransformationMatrix& camera_matrix) override;

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

    Player* player;
    State   currentState;
    double  timer;

    const double detectionRadius  = 500.0;
    const double warningDuration  = 2.0;
    const double cooldownDuration = 3.0;

    CS200::RGBA color = 0xFF0000FF; // 빨간색
};