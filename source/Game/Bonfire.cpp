#include "Game/Bonfire.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Game/Player.hpp"           // 플레이어 접근
#include "Game/WorldTextManager.hpp" // 텍스트 매니저

Bonfire::Bonfire(Math::vec2 start_pos, Math::vec2 size) : CS230::GameObject(start_pos), bonfireSize(size)
{
    Math::irect collision_box{
        { static_cast<int>(-size.x / 2.0), static_cast<int>(-size.y / 2.0) },
        {  static_cast<int>(size.x / 2.0),  static_cast<int>(size.y / 2.0) }
    };
    AddGOComponent(new CS230::RectCollision(collision_box, this));
}

void Bonfire::Draw(const Math::TransformationMatrix& camera_matrix)
{
    CS200::IRenderer2D&        renderer  = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(bonfireSize);

    // 모닥불을 '빨간 네모'로 그립니다.
    renderer.DrawRectangle(transform, 0xFF0000FF, CS200::CLEAR, 0.0);

    CS230::GameObject::Draw(camera_matrix);
}

void Bonfire::Interact(CS230::GameObject* interactor)
{
    auto textManager = Engine::GetGameStateManager().GetGSComponent<WorldTextManager>();

    // F키를 누른 *첫 프레임*에만 저장 로직을 실행
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::F))
    {
        Player* player = dynamic_cast<Player*>(interactor);
        if (player == nullptr)
            return;

        player->SetSavePoint(GetPosition());
        Engine::GetLogger().LogEvent("Event: Bonfire interacted, save point updated!");
    }

    // F키를 *누르고 있는 동안* 계속 "저장됨" 텍스트 표시
    if (textManager)
    {
        textManager->ShowTextAbove(this, "Checkpoint Saved.");
    }
}