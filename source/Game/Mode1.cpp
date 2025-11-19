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

    player = new Player({ 0.0, 800.0 });

    auto gom = GetGSComponent<CS230::GameObjectManager>();
    GetGSComponent<CS230::GameObjectManager>()->Add(player);

    Math::ivec2 winSize = Engine::GetWindow().GetSize();
    camera->SetPosition(player->GetPosition() - Math::vec2{ winSize.x * 0.5, winSize.y * 0.5 });

    // Star
    std::vector<TargetStar*> targetStars;

    TargetStar* t1 = new TargetStar({ 5500.0, 750.0 });
    gom->Add(t1);
    targetStars.push_back(t1);

    TargetStar* t2 = new TargetStar({ 4500.0, 750.0 });
    gom->Add(t2);
    targetStars.push_back(t2);

    TargetStar* t3 = new TargetStar({ 4800.0, 1000.0 });
    gom->Add(t3);
    targetStars.push_back(t3);

    TargetStar* t4 = new TargetStar({ 5200.0, 1000.0 });
    gom->Add(t4);
    targetStars.push_back(t4);

    TargetStar* t5 = new TargetStar({ 6500.0, 750.0 });
    gom->Add(t5);
    targetStars.push_back(t5);

    TargetStar* t6 = new TargetStar({ 7500.0, 750.0 });
    gom->Add(t6);
    targetStars.push_back(t6);

    Star* yellowStar = new Star({ 5000.0, 750.0 }, player, targetStars, StarType::Yellow);
    gom->Add(yellowStar);

    Star* redStar = new Star({ 7000.0, 750.0 }, player, targetStars, StarType::Red);
    gom->Add(redStar);

    Math::vec2 signSize = { 50.0, 25.0 };

    double platformY = 200;
    double signY     = platformY + (signSize.y / 2.0);

    double platformY2 = 500.0;
    double signY2     = platformY2 + (signSize.y / 2.0);

    gom->Add(new Sign({ 0.0, signY }, signSize, "A/D to Move"));
    gom->Add(new Sign({ 200.0, signY }, signSize, "W or Space to Jump"));

    Math::vec2 bonfireSize = { 25.0, 25.0 };
    double     bonfire1Y   = platformY2 + (bonfireSize.y / 2.0);
    gom->Add(new Bonfire({ 900.0, bonfire1Y }, bonfireSize));

    gom->Add(new Sign({ 800.0, signY2 }, signSize, "Press 'F' at Bonfire to Save"));
    gom->Add(new Sign({ 1900.0, signY2 }, signSize, "Press LShift to Dash"));
    gom->Add(new Sign({ 1100.0, signY }, signSize, "Press 'R' to Respawn"));
    gom->Add(new Sign({ 2700.0, signY }, signSize, "Hold 'LShift' to Sprint"));
    gom->Add(new Sign({ 4500.0, signY }, signSize, "Hold RMB: Shield (Reflects Light, Slows Move)"));
    gom->Add(new Sign({ 5000.0, signY }, signSize, "Reflect Light to Hit the Star!"));

    double bonfire2Y = platformY + (bonfireSize.y / 2.0);
    gom->Add(new Bonfire({ 6000.0, bonfire2Y }, bonfireSize));
    gom->Add(new Sign({ 5900.0, signY }, signSize, "Red Lasers Hurt! Parry with Timed Block! Be sure to Save at the Bonfire ahead!"));
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

    Math::TransformationMatrix view_projection_matrix = CS200::build_ndc_matrix(display_size_int) * camera->GetMatrix();
    renderer.BeginScene(view_projection_matrix);

    GetGSComponent<CS230::GameObjectManager>()->DrawAll(view_projection_matrix);

    renderer.EndScene();

    Math::TransformationMatrix screen_matrix = CS200::build_ndc_matrix(display_size_int);
    renderer.BeginScene(screen_matrix);

    if (worldTextManager != nullptr)
    {
        worldTextManager->Draw();
    }

    renderer.EndScene();
}

void Mode1::DrawImGui()
{
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
