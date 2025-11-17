#pragma once

#include "CS200/RGBA.hpp"
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

// IRenderer2D 전방 선언 추가
namespace CS200
{
    class IRenderer2D;
}

// 이 컴포넌트는 CS230 네임스페이스에 속하지 않습니다.
class WorldTextManager : public CS230::Component
{
public:
    WorldTextManager();
    void Update([[maybe_unused]] double dt) override; // 매 프레임 텍스트 목록을 초기화합니다.

    // ImGui 대신 2D 렌더러를 사용해 그립니다.
    void Draw();

    // 좌표 변환에 필요한 카메라를 설정합니다.
    void SetCamera(CS230::Camera* camera);

    // 오브젝트 위에 텍스트 표시를 요청합니다. (튜토리얼 자막용)
    void ShowTextAbove(CS230::GameObject* obj, const std::string& text, double scale, CS200::RGBA color);

    // 오브젝트 아래에 텍스트 표시를 요청합니다. ("Press 'F'" 프롬프트용)
    void ShowTextBelow(CS230::GameObject* obj, const std::string& text, double scale, CS200::RGBA color);

private:
    struct TextJob
    {
        std::string text;
        Math::vec2  worldPos;
        bool        alignAbove;
        double      scale;
        CS200::RGBA color;
    };

    // 월드 좌표를 렌더러의 스크린 좌표(0,0이 좌측 하단)로 변환합니다.
    Math::vec2 WorldToScreen(Math::vec2 worldPos);

    std::vector<TextJob> textJobs;
    CS230::Camera*       camera;
};