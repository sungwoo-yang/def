#pragma once

#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"

// CS230 네임스페이스를 사용하지 않습니다.
class Bonfire : public CS230::GameObject
{
public:
    Bonfire(Math::vec2 start_pos, Math::vec2 size);

    void Draw(const Math::TransformationMatrix& camera_matrix) override;
    
    GameObjectTypes Type() override { return GameObjectTypes::Bonfire; }
    std::string TypeName() override { return "Bonfire"; }

    // 플레이어가 상호작용 시 이 함수가 호출됩니다.
    void Interact(CS230::GameObject* interactor) override;

private:
    Math::vec2 bonfireSize;
};