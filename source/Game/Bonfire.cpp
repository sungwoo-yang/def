#include "Game/Bonfire.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/GameStateManager.hpp"
#include "Game/WorldTextManager.hpp" // 텍스트 매니저
#include "Game/Player.hpp"          // 플레이어 접근

Bonfire::Bonfire(Math::vec2 start_pos, Math::vec2 size) 
    : CS230::GameObject(start_pos), bonfireSize(size)
{
    Math::irect collision_box{
        { static_cast<int>(-size.x / 2.0), static_cast<int>(-size.y / 2.0) },
        { static_cast<int>( size.x / 2.0), static_cast<int>( size.y / 2.0) }
    };
    AddGOComponent(new CS230::RectCollision(collision_box, this));
}

void Bonfire::Draw(const Math::TransformationMatrix& camera_matrix)
{
    CS200::IRenderer2D& renderer = Engine::GetRenderer2D();
    Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(bonfireSize);
    
    // 모닥불을 '빨간 네모'로 그립니다.
    renderer.DrawRectangle(transform, 0xFF0000FF, CS200::CLEAR, 0.0);

    CS230::GameObject::Draw(camera_matrix); 
}

void Bonfire::Interact(CS230::GameObject* interactor)
{
    Player* player = dynamic_cast<Player*>(interactor);
    if (player == nullptr) return;

    // 플레이어의 저장 지점을 현재 모닥불 위치로 갱신
    player->SetSavePoint(GetPosition());

    auto textManager = Engine::GetGameStateManager().GetGSComponent<WorldTextManager>();
    if (textManager)
    {
        // 피드백 메시지를 오브젝트 '위에' 표시
        textManager->ShowTextAbove(this, "Checkpoint Saved.");
    }
    
    Engine::GetLogger().LogEvent("Event: Bonfire interacted, save point updated!");
}