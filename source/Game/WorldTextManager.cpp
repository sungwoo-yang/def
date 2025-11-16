#include "WorldTextManager.hpp"
#include "CS200/NDC.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Collision.hpp" // 충돌체 바운더리 확인용
#include "Engine/Engine.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Matrix.hpp" // Matrix 곱셈 연산자 사용
#include "Engine/Window.hpp"
#include <imgui.h>

WorldTextManager::WorldTextManager() : camera(nullptr)
{
}

void WorldTextManager::Update([[maybe_unused]] double dt)
{
    // 매 프레임 텍스트 목록을 비웁니다.
    // 텍스트는 매 프레임 ShowText...()가 호출되어야만 유지됩니다.
    textJobs.clear();
}

void WorldTextManager::SetCamera(CS230::Camera* cam)
{
    this->camera = cam;
}

Math::vec2 WorldTextManager::WorldToScreen(Math::vec2 worldPos)
{
    if (camera == nullptr)
        return { -1000, -1000 };

    // 1. Mode1::Draw()와 동일한 View-Projection Matrix를 계산합니다.
    Math::TransformationMatrix viewProj = CS200::build_ndc_matrix(Engine::GetWindow().GetSize()) * camera->GetMatrix();

    // 2. 월드 좌표(vec2)를 NDC 좌표(vec2)로 변환합니다.
    // (엔진의 TransformationMatrix * vec2 연산자 사용)
    Math::vec2 ndcPos = viewProj * worldPos;

    // 3. NDC (-1 ~ +1)를 ImGui 화면 좌표 (0 ~ WindowSize)로 변환합니다.
    // (ImGui의 Y좌표는 위에서 아래로 증가하므로 (1.0 - ndcPos.y) 사용)
    Math::ivec2 winSize = Engine::GetWindow().GetSize();
    double      screenX = (ndcPos.x + 1.0f) * 0.5f * winSize.x;
    double      screenY = (1.0f - ndcPos.y) * 0.5f * winSize.y;

    return { screenX, screenY };
}

// 오브젝트의 충돌체 상단 10px 위에 텍스트 표시
void WorldTextManager::ShowTextAbove(CS230::GameObject* obj, const std::string& text)
{
    if (obj == nullptr)
        return;

    Math::vec2 worldPos = obj->GetPosition();
    // 충돌체 정보를 가져와서 정확한 상단 위치를 계산합니다.
    auto       collider = obj->GetGOComponent<CS230::RectCollision>();
    if (collider)
    {
        worldPos.x = obj->GetPosition().x;                   // 중심 X 사용
        worldPos.y = collider->WorldBoundary().Top() + 10.0; // 충돌체 상단 + 10px
    }

    textJobs.push_back({ text, worldPos, true });
}

// 오브젝트의 충돌체 하단 10px 아래에 텍스트 표시
void WorldTextManager::ShowTextBelow(CS230::GameObject* obj, const std::string& text)
{
    if (obj == nullptr)
        return;

    Math::vec2 worldPos = obj->GetPosition();
    auto       collider = obj->GetGOComponent<CS230::RectCollision>();
    if (collider)
    {
        worldPos.x = obj->GetPosition().x;                      // 중심 X 사용
        worldPos.y = collider->WorldBoundary().Bottom() - 10.0; // 충돌체 하단 - 10px
    }

    textJobs.push_back({ text, worldPos, false });
}

void WorldTextManager::DrawImGui()
{
    if (textJobs.empty())
        return;

    // ImGui의 포그라운드 드로우 리스트를 사용하여 텍스트를 직접 그립니다.
    ImDrawList* draw_list = ImGui::GetForegroundDrawList();

    for (const auto& job : textJobs)
    {
        Math::vec2 screenPos = WorldToScreen(job.worldPos);
        ImVec2     textSize  = ImGui::CalcTextSize(job.text.c_str());

        // X축 중앙 정렬
        ImVec2 textPos;
        textPos.x = static_cast<float>(screenPos.x) - textSize.x * 0.5f;

        // Y축 정렬
        if (job.alignAbove)
        {
            textPos.y = static_cast<float>(screenPos.y) - textSize.y; // 텍스트의 하단이 worldPos에 오도록
        }
        else
        {
            textPos.y = static_cast<float>(screenPos.y); // 텍스트의 상단이 worldPos에 오도록
        }

        // 가독성을 위해 검은색 테두리(그림자)를 먼저 그립니다.
        draw_list->AddText(ImVec2(textPos.x + 1, textPos.y + 1), IM_COL32(0, 0, 0, 255), job.text.c_str());
        // 흰색 텍스트를 그립니다.
        draw_list->AddText(textPos, IM_COL32(255, 255, 255, 255), job.text.c_str());
    }
}