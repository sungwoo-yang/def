#include "Game/Sign.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Input.hpp"
#include "Game/WorldTextManager.hpp"

Sign::Sign(Math::vec2 start_pos, Math::vec2 size, std::string msg) : CS230::GameObject(start_pos), signSize(size), message(std::move(msg)) // msg를 message 멤버에 저장
{
    Math::irect collision_box{
        { static_cast<int>(-size.x / 2.0), static_cast<int>(-size.y / 2.0) },
        {  static_cast<int>(size.x / 2.0),  static_cast<int>(size.y / 2.0) }
    };
    // 엔진의 컴포넌트를 네임스페이스와 함께 사용
    AddGOComponent(new CS230::RectCollision(collision_box, this));
}

void Sign::Draw(const Math::TransformationMatrix& camera_matrix)
{
    CS200::IRenderer2D&        renderer  = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(signSize);

    // 표지판을 노란색 사각형으로 그립니다.
    renderer.DrawRectangle(transform, 0xFFFF00FF, CS200::CLEAR, 0.0);

    CS230::GameObject::Draw(camera_matrix);
}

// "이벤트"의 실체
void Sign::Interact([[maybe_unused]] CS230::GameObject* interactor)
{
    // GameStateManager를 통해 WorldTextManager 컴포넌트를 가져옵니다.
    auto textManager = Engine::GetGameStateManager().GetGSComponent<WorldTextManager>();
    if (textManager)
    {
        // 튜토리얼 메시지를 오브젝트 '위에' 표시
        textManager->ShowTextAbove(this, message);
    }

    // F키를 누른 첫 프레임에만 로그를 남깁니다.
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::F))
    {
        Engine::GetLogger().LogEvent("Event: Sign interacted, showing: " + message);
    }
}