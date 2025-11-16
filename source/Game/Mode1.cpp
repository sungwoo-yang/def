#include "Mode1.h"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/MapManager.h"
#include "Engine/ShowCollision.hpp"
#include "Engine/Window.hpp"
#include "Player.hpp"
#include "Sign.hpp"
#include "SubtitleManager.hpp"
#include <imgui.h>

void Mode1::Load()
{
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new CS230::ShowCollision());

    subtitleManager = new CS230::SubtitleManager();
    AddGSComponent(subtitleManager);

    camera = new CS230::Camera(
        Math::rect{
            {   0,   0 },
            { 800, 600 }
    });
    AddGSComponent(camera);

    mapManager = new CS230::MapManager();
    mapManager->AddMap(new CS230::Map("Assets/maps/Tutorial.svg"));
    mapManager->LoadMap();
    AddGSComponent(mapManager);

    // add player
    player = new Player({ 0.0, 1600.0 });
    GetGSComponent<CS230::GameObjectManager>()->Add(player);
    camera->SetPosition(player->GetPosition() - Math::vec2{ 400, 300 });

    // 요청하신 표지판들을 생성합니다. (Y 좌표는 플랫폼 높이에 맞게 조정해야 합니다)
    auto       gom      = GetGSComponent<CS230::GameObjectManager>();
    Math::vec2 signSize = { 50, 25.0 }; // 표지판 기본 크기

    // SVG 로딩이 실제로는 비어있으므로, y=300을 플랫폼 상단으로 가정하고 배치합니다.
    double platformY = 200;
    double signY     = platformY + 50; // 표지판이 플랫폼 위에 서 있도록 Y 위치 계산

    gom->Add(new CS230::Sign({ 0.0, signY }, signSize, "AD keys"));
    gom->Add(new CS230::Sign({ 100.0, signY }, signSize, "W or space"));
    gom->Add(new CS230::Sign({ 900.0, signY + 200.0 }, signSize, "fire")); // 다른 높이의 플랫폼
    gom->Add(new CS230::Sign({ 1900.0, signY + 200.0 }, signSize, "LShift Dash"));
}

void Mode1::Update(double dt)
{
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);

    if (player != nullptr)
    {
        Math::vec2 targetPos = player->GetPosition() - Math::vec2{ Engine::GetWindow().GetSize().x / 2.0, Engine::GetWindow().GetSize().y / 2.0 };
        camera->SetPosition(targetPos);
    }

    GetGSComponent<CS230::GameObjectManager>()->CollisionTest();
}

void Mode1::Draw()
{
    CS200::IRenderer2D& renderer = Engine::GetRenderer2D();
    Engine::GetWindow().Clear(CS200::BLACK);
    Math::ivec2                display_size_int       = Engine::GetWindow().GetSize();
    Math::TransformationMatrix view_projection_matrix = CS200::build_ndc_matrix(display_size_int) * camera->GetMatrix();
    renderer.BeginScene(view_projection_matrix);
    GetGSComponent<CS230::GameObjectManager>()->DrawAll(view_projection_matrix);
    renderer.EndScene();
}

void Mode1::DrawImGui()
{
    if (player != nullptr && ImGui::Begin("Mode1 Player Debug"))
    {
        ImGui::Text("Player Position: (%.1f, %.1f)", player->GetPosition().x, player->GetPosition().y);
        ImGui::Text("Player Y Velocity: %.1f", player->velocityY);
        ImGui::Text("Is Jumping/Falling: %s", player->isJumping ? "Yes" : "No");

        // currentPlatformIndex는 현재 단순 플래그 역할만 함
        if (!player->isJumping)
        {
            ImGui::Text("On Platform: Yes");
        }
        else
        {
            ImGui::Text("On Platform: No");
        }

        ImGui::Separator();
        ImGui::Text("Is Dashing: %s", player->dashComponent.IsDashing() ? "Yes" : "No");
        ImGui::Text("Is Sprinting: %s", player->isSprinting ? "Yes" : "No");
        ImGui::Text("Dash Cooldown: %.2f", std::max(0.0, player->dashComponent.dashCooldownTimer));
        ImGui::Text("Shift Hold Time: %.2f", player->shiftHoldTimer);
    }
    ImGui::End();

    if (subtitleManager != nullptr)
    {
        subtitleManager->Draw();
    }
}

void Mode1::Unload()
{
    ClearGSComponents();
    player          = nullptr;
    mapManager      = nullptr;
    subtitleManager = nullptr;
}
