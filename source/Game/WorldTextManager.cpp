#include "WorldTextManager.hpp"
#include "CS200/NDC.hpp"
#include "Engine/Camera.hpp"
#include "Engine/Collision.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObject.hpp"
#include "Engine/Matrix.hpp"
#include "Engine/Window.hpp"

// ImGui 대신 엔진 폰트와 텍스처, 렌더러 헤더를 포함합니다.
#include "CS200/IRenderer2D.hpp"
#include "Engine/Font.hpp"
#include "Engine/Texture.hpp"

WorldTextManager::WorldTextManager() : camera(nullptr)
{
}

void WorldTextManager::Update([[maybe_unused]] double dt)
{
    // 매 프레임 텍스트 목록을 비웁니다.
    textJobs.clear();
}

void WorldTextManager::SetCamera(CS230::Camera* cam)
{
    this->camera = cam;
}

// 렌더러의 스크린 좌표계(Y-Up)로 변환하도록 수정
Math::vec2 WorldTextManager::WorldToScreen(Math::vec2 worldPos)
{
    if (camera == nullptr)
    {
        return { -1000.0, -1000.0 }; // double 타입 사용
    }

    // 1. 월드 -> NDC 변환
    Math::TransformationMatrix viewProj = CS200::build_ndc_matrix(Engine::GetWindow().GetSize()) * camera->GetMatrix();
    Math::vec2                 ndcPos   = viewProj * worldPos; // vec2는 double 타입
    Math::ivec2                winSize  = Engine::GetWindow().GetSize();

    // 2. NDC -> 스크린 좌표 (0,0이 좌측 하단)
    // C++20 스타일로 static_cast 사용 및 double로 연산
    double screenX = (ndcPos.x + 1.0) * 0.5 * static_cast<double>(winSize.x);
    double screenY = (ndcPos.y + 1.0) * 0.5 * static_cast<double>(winSize.y);

    return { screenX, screenY };
}

// ShowTextAbove 함수는 변경 없습니다.
void WorldTextManager::ShowTextAbove(CS230::GameObject* obj, const std::string& text, double scale, CS200::RGBA color)
{
    if (obj == nullptr)
        return;

    Math::vec2 pos      = obj->GetPosition();
    auto       collider = obj->GetGOComponent<CS230::RectCollision>();
    if (collider)
    {
        pos.x = obj->GetPosition().x;                   // 중심 X 사용
        pos.y = collider->WorldBoundary().Top() + 15.0; // 충돌체 상단 + 15px
    }

    textJobs.push_back({ text, pos, true, scale, color });
}

// ShowTextBelow 함수는 변경 없습니다.
void WorldTextManager::ShowTextBelow(CS230::GameObject* obj, const std::string& text, double scale, CS200::RGBA color)
{
    if (obj == nullptr)
        return;

    Math::vec2 pos      = obj->GetPosition();
    auto       collider = obj->GetGOComponent<CS230::RectCollision>();
    if (collider)
    {
        pos.x = obj->GetPosition().x;                      // 중심 X 사용
        pos.y = collider->WorldBoundary().Bottom() - 15.0; // 충돌체 하단 - 15px
    }

    textJobs.push_back({ text, pos, false, scale, color });
}

// ImGui::DrawImGui() 대신 2D 렌더러를 사용하는 Draw() 함수로 구현
void WorldTextManager::Draw()
{
    if (textJobs.empty())
    {
        return;
    }

    // 엔진의 폰트 리소스 (main.cpp에서 0번으로 "Font_Simple.png"가 로드됨)
    CS230::Font& font = Engine::GetFont(0);

    for (const auto& job : textJobs)
    {
        // 1. 텍스트를 텍스처로 렌더링 (엔진 폰트 기능 사용)
        std::shared_ptr<CS230::Texture> textTexture = font.PrintToTexture(job.text, job.color);

        if (textTexture == nullptr)
        {
            continue; // 텍스처 생성 실패
        }

        // 2. 텍스처 크기와 변환된 스크린 좌표 가져오기
        Math::ivec2 textureSize = textTexture->GetSize();
        Math::vec2  screenPos   = WorldToScreen(job.worldPos); // Y-Up 스크린 좌표

        const double scale = job.scale;

        // 3. DemoText.cpp와 동일하게 텍스처의 '좌측 하단' 위치 계산
        double scaledWidth  = static_cast<double>(textureSize.x) * scale;
        double scaledHeight = static_cast<double>(textureSize.y) * scale;

        Math::vec2 drawPos_BottomLeft; // 최종적으로 텍스처가 그려질 좌측 하단 위치
        drawPos_BottomLeft.x = screenPos.x - scaledWidth * 0.5;

        if (job.alignAbove)
        {
            // 텍스트의 하단을 job.worldPos의 스크린 Y좌표에 맞춤
            drawPos_BottomLeft.y = screenPos.y;
        }
        else // alignBelow
        {
            // 텍스트의 상단을 job.worldPos의 스크린 Y좌표에 맞춤
            drawPos_BottomLeft.y = screenPos.y - scaledHeight;
        }

        // 4. DemoText::drawText와 동일하게 변환 행렬 생성
        Math::TransformationMatrix transform = Math::TranslationMatrix(drawPos_BottomLeft) * Math::ScaleMatrix(Math::vec2{ scale, scale });

        // 5. Texture::Draw 호출 (내부적으로 IRenderer2D::DrawQuad 호출)
        // 텍스처는 흰색으로 생성했으므로 틴트 없이 흰색으로 그립니다.
        textTexture->Draw(transform, CS200::WHITE);
    }
}