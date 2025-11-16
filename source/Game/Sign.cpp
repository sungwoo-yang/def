#include "Game/Sign.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/RGBA.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameStateManager.hpp"
#include "Engine/Logger.hpp"
#include "Game/SubTitleManager.hpp"

namespace CS230
{
    Sign::Sign(Math::vec2 start_pos, Math::vec2 size, std::string msg) : GameObject(start_pos), signSize(size), message(std::move(message))
    {
        // 플레이어가 이 표지판 근처에 있는지 감지하기 위한 충돌 영역
        // 크기의 절반을 사용 (중심점 기준)
        Math::irect collision_box{
            { static_cast<int>(-size.x / 2.0), static_cast<int>(-size.y / 2.0) },
            {  static_cast<int>(size.x / 2.0),  static_cast<int>(size.y / 2.0) }
        };
        AddGOComponent(new RectCollision(collision_box, this));
    }

    void Sign::Update(double dt)
    {
        GameObject::Update(dt);
    }

    void Sign::Draw(const Math::TransformationMatrix& camera_matrix)
    {
        // 디버깅용으로 표지판을 노란색 사각형으로 그립니다.
        CS200::IRenderer2D&        renderer  = Engine::GetRenderer2D();
        Math::TransformationMatrix transform = GetMatrix() * Math::ScaleMatrix(signSize);

        // 사각형 그리기 (채우기: 노란색, 외곽선: 없음)
        renderer.DrawRectangle(transform, 0xFFFF00FF, CS200::CLEAR, 0.0);

        // (선택 사항) 충돌 영역 그리기 (ShowCollision 컴포넌트가 활성화되었을 때)
        GameObject::Draw(camera_matrix);
    }

    // "이벤트"의 실체
    void Sign::Interact([[maybe_unused]] GameObject* interactor)
    {
        // GameStateManager를 통해 SubtitleManager 컴포넌트를 가져옵니다.
        auto subtitleManager = Engine::GetGameStateManager().GetGSComponent<SubtitleManager>();
        if (subtitleManager)
        {
            // 자신의 메시지를 SubtitleManager에게 전달합니다.
            subtitleManager->ShowMessage(message);
        }

        Engine::GetLogger().LogEvent("Event: Sign interacted, showing: " + message);
    }
}