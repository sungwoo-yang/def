#pragma once

#include "Engine/GameObject.hpp"
#include "Engine/GameObjectTypes.hpp"
#include <string>

namespace CS230
{
    class Sign : public GameObject
    {
    public:
        Sign(Math::vec2 start_pos, Math::vec2 size, std::string msg);

        void Update(double dt) override;
        void Draw(const Math::TransformationMatrix& camera_matrix) override;

        // GameObject 가상 함수 오버라이드
        GameObjectTypes Type() override
        {
            return GameObjectTypes::Sign;
        }

        std::string TypeName() override
        {
            return "Sign";
        }

        // 플레이어가 이 오브젝트와 상호작용할 때 호출될 함수
        void Interact(GameObject* interactor) override;

    private:
        Math::vec2  signSize;
        std::string message;
    };
}