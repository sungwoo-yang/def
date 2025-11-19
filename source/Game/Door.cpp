#include "Door.hpp"
#include "Mode2.hpp"  // Mode2로 전환하기 위해 필요
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Game/WorldTextManager.hpp"

Door::Door(Math::vec2 position, Math::vec2 size) : CS230::GameObject(position), size(size)
{
    // 충돌 박스 추가
    Math::irect collision_box{
        { static_cast<int>(-size.x / 2.0), static_cast<int>(-size.y / 2.0) },
        { static_cast<int>(size.x / 2.0),  static_cast<int>(size.y / 2.0) }
    };
    AddGOComponent(new CS230::RectCollision(collision_box, this));
}

void Door::Draw(const Math::TransformationMatrix& camera_matrix)
{
    CS200::IRenderer2D& renderer = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(size);

    // 문은 갈색(Brown-ish)으로 그리기
    renderer.DrawRectangle(transform, 0x8B4513FF, CS200::WHITE, 2.0);
    CS230::GameObject::Draw(camera_matrix);
}

void Door::Interact([[maybe_unused]]CS230::GameObject* interactor)
{
    // 텍스트 띄우기
    auto textManager = Engine::GetGameStateManager().GetGSComponent<WorldTextManager>();
    if (textManager)
    {
        textManager->ShowTextAbove(this, "Press 'F' to Enter", 0.6, CS200::WHITE);
    }

    // F키 입력 확인
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::F))
    {
        Engine::GetLogger().LogEvent("Entering Door -> Mode2");
        
        // 현재 상태들을 모두 정리하고 Mode2로 전환
        Engine::GetGameStateManager().Clear();
        Engine::GetGameStateManager().PushState<Mode2>();
    }
}