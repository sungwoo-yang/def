#pragma once

#include "Engine/Component.hpp"
#include "Engine/Vec2.hpp"
#include <string>
#include <vector>

namespace CS230
{
    class Camera; // 전방 선언

    // 프롬프트 정보를 저장할 구조체
    struct Prompt
    {
        std::string text;
        Math::vec2  worldPosition; // 오브젝트의 월드 좌표
    };

    class PromptManager : public Component
    {
    public:
        PromptManager();

        // 매 프레임 시작 시 프롬프트 목록을 비웁니다.
        void Update(double dt) override;

        // ImGui를 사용해 프롬프트를 그립니다.
        void DrawImGui();

        // 현재 프레임에 표시할 프롬프트를 추가합니다.
        void ShowPrompt(const std::string& text, Math::vec2 worldPosition);

        // 좌표 변환에 필요한 카메라를 설정합니다.
        void SetCamera(Camera* camera);

    private:
        // 월드 좌표를 ImGui가 사용하는 화면 좌표로 변환합니다.
        Math::vec2 WorldToScreen(Math::vec2 worldPos);
        
        std::vector<Prompt> prompts;
        Camera* camera;
    };
}