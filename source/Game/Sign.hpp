#pragma once

#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include <string>

// CS230 네임스페이스를 사용하지 않습니다.
class Sign : public CS230::GameObject
{
public:
    // 생성자에서 msg를 받습니다.
    Sign(Math::vec2 start_pos, Math::vec2 size, std::string msg);

    void Draw(const Math::TransformationMatrix& camera_matrix) override;

    GameObjectTypes Type() override
    {
        return GameObjectTypes::Sign;
    }

    std::string TypeName() override
    {
        return "Sign";
    }

    // 플레이어가 상호작용 시 이 함수가 호출됩니다.
    void Interact(CS230::GameObject* interactor) override;

private:
    Math::vec2  signSize;
    std::string message; // 'msg'를 저장할 멤버 변수
};