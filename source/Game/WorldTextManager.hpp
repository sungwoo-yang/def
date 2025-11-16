#pragma once

#include "Engine/Component.hpp"
#include "Engine/Vec2.hpp"
#include <string>
#include <vector>

// 엔진 네임스페이스의 클래스 전방 선언
namespace CS230
{
    class Camera;
    class GameObject;
}

// 이 컴포넌트는 CS230 네임스페이스에 속하지 않습니다.
class WorldTextManager : public CS230::Component
{
public:
    WorldTextManager();
    void Update([[maybe_unused]]double dt) override; // 매 프레임 텍스트 목록을 초기화합니다.
    void DrawImGui(); // ImGui를 사용해 텍스트를 그립니다.

    // 좌표 변환에 필요한 카메라를 설정합니다.
    void SetCamera(CS230::Camera* camera);

    // 오브젝트 위에 텍스트 표시를 요청합니다. (튜토리얼 자막용)
    void ShowTextAbove(CS230::GameObject* obj, const std::string& text);
    
    // 오브젝트 아래에 텍스트 표시를 요청합니다. ("Press 'F'" 프롬프트용)
    void ShowTextBelow(CS230::GameObject* obj, const std::string& text);

private:
    // 그려야 할 텍스트 작업을 저장하는 구조체
    // (worldPosition이 아닌 worldPos를 사용합니다)
    struct TextJob
    {
        std::string text;
        Math::vec2  worldPos;   // 텍스트가 표시될 월드 좌표
        bool        alignAbove; // true: 텍스트를 worldPos 위에, false: 텍스트를 worldPos 아래에
    };

    // 월드 좌표를 ImGui가 사용하는 화면 좌표로 변환합니다.
    Math::vec2 WorldToScreen(Math::vec2 worldPos);
    
    std::vector<TextJob> textJobs;
    CS230::Camera* camera;
};