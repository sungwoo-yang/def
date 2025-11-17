#pragma once
#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include "Engine/Vec2.hpp"
#include "CS200/RGBA.hpp"
#include <vector>
#include <utility> // std::pair

class Player; // 전방 선언

class YellowLaser : public CS230::GameObject
{
public:
    // 생성자에 Player* 추가 (쉴드 정보 접근 및 반사 계산용)
    YellowLaser(Math::vec2 startPos, Math::vec2 direction, Player* targetPlayer);

    void Update(double dt) override;
    void Draw(const Math::TransformationMatrix& camera_matrix) override;

    GameObjectTypes Type() override { return GameObjectTypes::Laser; }
    std::string TypeName() override { return "YellowLaser"; }

private:
    Math::vec2 startPosition;
    Math::vec2 direction;
    Player* player; // 플레이어(쉴드) 참조

    double currentDistance = 0.0; // 레이저 머리 이동 거리
    
    const double speed = 600.0;     // 속도
    const double length = 300.0;    // 뱀 길이
    const double maxRange = 4000.0; // 최대 사거리

    // 계산된 전체 경로 (반사 포함)
    std::vector<std::pair<Math::vec2, Math::vec2>> fullPath;
};