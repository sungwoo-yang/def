#include "SubtitleManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include <imgui.h>

namespace CS230
{
    SubtitleManager::SubtitleManager() : displayTimer(0.0)
    {
    }

    void SubtitleManager::Update(double dt)
    {
        if (displayTimer > 0.0)
        {
            displayTimer -= dt;
            if (displayTimer <= 0.0)
            {
                currentMessage.clear(); // 시간이 다 되면 메시지 삭제
            }
        }
    }

    void SubtitleManager::ShowMessage(const std::string& message, double duration)
    {
        currentMessage = message;
        displayTimer = duration;
    }

    void SubtitleManager::Draw()
    {
        if (displayTimer <= 0.0 || currentMessage.empty())
        {
            return; // 표시할 메시지가 없으면 아무것도 하지 않음
        }

        // ImGui를 사용하여 화면 하단 중앙에 반투명 창으로 텍스트를 그립니다.
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.9f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowBgAlpha(0.5f); // 반투명 배경
        ImGui::Begin("Subtitles", nullptr, 
            ImGuiWindowFlags_NoTitleBar | 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_AlwaysAutoResize | 
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav);
        
        // 텍스트 크기를 1.5배 키웁니다.
        ImGui::SetWindowFontScale(1.5f);
        ImGui::Text("%s", currentMessage.c_str());
        ImGui::SetWindowFontScale(1.0f); // 폰트 스케일 리셋
        
        ImGui::End();
    }
}