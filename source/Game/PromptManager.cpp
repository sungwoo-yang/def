#include "PromptManager.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Window.hpp"
#include "Engine/Camera.hpp"
#include "CS200/NDC.hpp"
#include "Engine/Matrix.hpp" // Matrix 곱셈 연산자 사용
#include <imgui.h>

namespace CS230
{
    PromptManager::PromptManager() : camera(nullptr)
    {
    }

    void PromptManager::Update(double dt)
    {
        // 매 프레임 목록을 비웁니다.
        // (충돌 중인 프레임에만 ShowPrompt가 호출되어 텍스트가 유지됨)
        prompts.clear();
    }

    void PromptManager::SetCamera(Camera* camera)
    {
        this->camera = camera;
    }

    void PromptManager::ShowPrompt(const std::string& text, Math::vec2 worldPosition)
    {
        prompts.push_back({ text, worldPosition });
    }

    Math::vec2 PromptManager::WorldToScreen(Math::vec2 worldPos)
    {
        if (camera == nullptr)
        {
            return { -1000, -1000 }; // 카메라가 없으면 그리지 않음
        }

        // 1. 월드 좌표 -> 뷰 좌표 -> NDC 좌표
        // Mode1::Draw()와 동일한 변환 행렬을 사용합니다.
        Math::TransformationMatrix viewProj = 
            CS200::build_ndc_matrix(Engine::GetWindow().GetSize()) * camera->GetMatrix();

        // 2. vec2와 3x3 Matrix 곱셈 (엔진에 구현된 연산자 사용)
        // (x, y, 1) 벡터로 변환하여 계산한 후 (x', y')를 반환합니다.
        Math::vec2 ndcPos = viewProj * worldPos;

        // 3. NDC (-1 ~ +1) -> ImGui 화면 좌표 (0 ~ WindowSize)
        // (ImGui는 Y좌표가 위에서 아래로 증가하므로 (1.0 - ndcPos.y) 사용)
        Math::ivec2 winSize = Engine::GetWindow().GetSize();
        float screenX = (ndcPos.x + 1.0f) * 0.5f * winSize.x;
        float screenY = (1.0f - ndcPos.y) * 0.5f * winSize.y;

        return { screenX, screenY };
    }

    void PromptManager::DrawImGui()
    {
        if (prompts.empty())
        {
            return;
        }

        // ImGui의 포그라운드 드로우 리스트를 사용하여 텍스트를 직접 그립니다.
        // 이 방식이 창을 띄우는 것보다 더 가볍고 안정적입니다.
        ImDrawList* draw_list = ImGui::GetForegroundDrawList();

        for (const auto& prompt : prompts)
        {
            Math::vec2 screenPos = WorldToScreen(prompt.worldPosition);

            // 텍스트 크기를 계산하여 중앙 정렬
            ImVec2 textSize = ImGui::CalcTextSize(prompt.text.c_str());
            ImVec2 textPos = ImVec2(screenPos.x - textSize.x * 0.5f, screenPos.y);

            // 검은색 배경(테두리) 먼저 그려서 가독성 확보
            draw_list->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), prompt.text.c_str());
            // 흰색 텍스트
            draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), prompt.text.c_str());
        }
    }
}