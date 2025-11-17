#include "Mode1.h"
#include "Bonfire.hpp"
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
#include "Star.hpp"
#include "TargetStar.hpp"
#include "WorldTextManager.hpp"
#include <imgui.h>

void Mode1::Load()
{
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new CS230::ShowCollision());

    camera = new CS230::Camera(
        Math::rect{
            {   0,   0 },
            { 800, 600 }
    });
    AddGSComponent(camera);

    worldTextManager = new WorldTextManager();
    worldTextManager->SetCamera(camera);
    AddGSComponent(worldTextManager);

    mapManager = new CS230::MapManager();
    mapManager->AddMap(new CS230::Map("Assets/maps/Tutorial.svg"));
    mapManager->LoadMap();
    AddGSComponent(mapManager);

    // add player
    player = new Player({ 0.0, 800.0 });

    auto gom = GetGSComponent<CS230::GameObjectManager>();
    GetGSComponent<CS230::GameObjectManager>()->Add(player);

    Math::ivec2 winSize = Engine::GetWindow().GetSize();
    camera->SetPosition(player->GetPosition() - Math::vec2{ winSize.x * 0.5, winSize.y * 0.5 });

    // STAR
    TargetStar* destStar = new TargetStar({ 5500.0, 750.0 });
    gom->Add(destStar);

    Star* yellowStar = new Star({ 5000.0, 750.0 }, player, destStar);
    gom->Add(yellowStar);

    // 요청하신 표지판들을 생성합니다. (Y 좌표는 플랫폼 높이에 맞게 조정해야 합니다)
    Math::vec2 signSize = { 50.0, 25.0 }; // 표지판 기본 크기

    // SVG 로딩이 실제로는 비어있으므로, y=300을 플랫폼 상단으로 가정하고 배치합니다.
    double platformY = 200;
    double signY     = platformY + (signSize.y / 2.0); // 표지판이 플랫폼 위에 서 있도록 Y 위치 계산

    double platformY2 = 500.0;
    double signY2     = platformY2 + (signSize.y / 2.0);

    gom->Add(new Sign({ 0.0, signY }, signSize, "AD keys to move"));
    gom->Add(new Sign({ 200.0, signY }, signSize, "W or Space to Jump"));

    gom->Add(new Sign({ 1900.0, signY2 }, signSize, "LShift to Dash"));

    // 모닥불 (Bonfire)
    Math::vec2 bonfireSize = { 25.0, 25.0 };
    double     bonfireY    = platformY2 + (bonfireSize.y / 2.0);
    gom->Add(new Bonfire({ 900.0, bonfireY }, bonfireSize));

    // 모닥불 튜토리얼 표지판
    gom->Add(new Sign({ 800.0, signY2 }, signSize, "Press 'F' at Bonfire to save."));

    gom->Add(new Sign({ 1100.0, signY }, signSize, "Press 'R' to Respawn"));
    gom->Add(new Sign({ 2700.0, signY }, signSize, "Hold 'LShift' to Sprint"));
    gom->Add(new Sign({ 4500.0, signY }, signSize, "Space to Parry"));
}

void Mode1::Update(double dt)
{
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);

    GetGSComponent<CS230::GameObjectManager>()->CollisionTest();

    if (player != nullptr && player->interactionTarget == nullptr)
    {
        player->isInteracting = false;
    }

    if (player != nullptr)
    {
        Math::vec2 winSize   = static_cast<Math::vec2>(Engine::GetWindow().GetSize());
        Math::vec2 targetPos = player->GetPosition() - Math::vec2{ winSize.x * 0.5, winSize.y * 0.3 };
        camera->SetPosition(targetPos);
    }

    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::P))
    {
        player->SetPosition({ 5000, 300 });
    }
}

void Mode1::Draw()
{
    CS200::IRenderer2D& renderer = Engine::GetRenderer2D();
    Engine::GetWindow().Clear(CS200::BLACK);
    Math::ivec2 display_size_int = Engine::GetWindow().GetSize();

    // --- 1. 월드 좌표계 렌더링 (카메라 적용) ---
    Math::TransformationMatrix view_projection_matrix = CS200::build_ndc_matrix(display_size_int) * camera->GetMatrix();
    renderer.BeginScene(view_projection_matrix);

    GetGSComponent<CS230::GameObjectManager>()->DrawAll(view_projection_matrix);

    renderer.EndScene();

    // --- 2. 스크린 좌표계 렌더링 (카메라 미적용) ---
    // (DemoText.cpp와 동일한 방식)
    Math::TransformationMatrix screen_matrix = CS200::build_ndc_matrix(display_size_int);
    renderer.BeginScene(screen_matrix);

    // WorldTextManager가 스크린 좌표계에서 텍스트를 그리도록 호출
    if (worldTextManager != nullptr)
    {
        worldTextManager->Draw();
    }

    renderer.EndScene();
}

void Mode1::DrawImGui()
{
    // 플레이어 디버그 창 (ImGui)
    if (player != nullptr && ImGui::Begin("Mode1 Player Debug"))
    {
        ImGui::Text("Player Position: (%.1f, %.1f)", player->GetPosition().x, player->GetPosition().y);
        ImGui::Text("Player Y Velocity: %.1f", player->velocityY);
        ImGui::Text("Is Jumping/Falling: %s", player->isJumping ? "Yes" : "No");
        ImGui::Text("Interaction Target: %s", player->interactionTarget ? player->interactionTarget->TypeName().c_str() : "None");

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
}

void Mode1::Unload()
{
    ClearGSComponents();
    player           = nullptr;
    mapManager       = nullptr;
    worldTextManager = nullptr;
}
