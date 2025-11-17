#pragma once
#include "CS200/RGBA.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

class Player; // 전방 선언
class TargetStar;

enum class StarType
{
    Yellow, // 뱀 레이저 (가드 반사)
    Red     // 즉발 레이저 (패링 반사)
};

class Star : public CS230::GameObject
{
public:
    Star(Math::vec2 position, Player* targetPlayer, const std::vector<TargetStar*>& destStars, StarType type);
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
        Idle,     // 감지 대기
        Warning,  // 2초간 조준 (보조선)
        Cooldown, // 발사 후 대기
    };

    Player*                  player;
    std::vector<TargetStar*> targets;

    State  currentState;
    double timer;

    StarType starType;

    const double detectionRadius  = 500.0;
    const double warningDuration  = 3.0;
    const double cooldownDuration = 5.0;
    const double parryWindowTime  = 0.5;

    CS200::RGBA color = 0xFF0000FF; // 빨간색
};