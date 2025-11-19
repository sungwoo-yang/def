#include "Mode1.hpp"
#include "Bonfire.hpp"
#include "CS200/IRenderer2D.hpp"
#include "CS200/NDC.hpp"
#include "Door.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Font.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/Input.hpp"
#include "Engine/Logger.hpp"
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
    currentState = State::Loading;

    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new CS230::ShowCollision());

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

    /*
    worldTextManager = new WorldTextManager();
    worldTextManager->SetCamera(camera);
    AddGSComponent(worldTextManager);


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

    double platformY  = 200;
    double platformY2 = 500.0;

    // Sign
    Math::vec2 signSize = { 50.0, 25.0 };

    double sign1_y = platformY + (signSize.y / 2.0);
    double sign2_y = platformY2 + (signSize.y / 2.0);

    double sign1_x  = 0.0;
    double sign2_x  = 200.0;
    double sign3_x  = 800.0;
    double sign4_x  = 1100.0;
    double sign5_x  = 1900.0;
    double sign6_x  = 2700.0;
    double sign7_x  = 4500.0;
    double sign8_x  = 5000.0;
    double sign9_x  = 5900.0;
    double sign10_x = 7900.0;

    gom->Add(new Sign({ sign1_x, sign1_y }, signSize, "A/D to Move"));
    gom->Add(new Sign({ sign2_x, sign1_y }, signSize, "W or Space to Jump"));
    gom->Add(new Sign({ sign3_x, sign2_y }, signSize, "Press 'F' at Bonfire to Save"));
    gom->Add(new Sign({ sign4_x, sign1_y }, signSize, "Press 'R' to Respawn"));
    gom->Add(new Sign({ sign5_x, sign2_y }, signSize, "Press LShift to Dash"));
    gom->Add(new Sign({ sign6_x, sign1_y }, signSize, "Hold 'LShift' to Sprint"));
    gom->Add(new Sign({ sign7_x, sign1_y }, signSize, "Hold RMB: Shield (Reflects Light, Slows Move)"));
    gom->Add(new Sign({ sign8_x, sign1_y }, signSize, "Reflect Light to Hit the Star!"));
    gom->Add(new Sign({ sign9_x, sign1_y }, signSize, "Red Lasers Hurt! Parry with Timed Block! Be sure to Save at the Bonfire ahead!"));
    gom->Add(new Sign({ sign10_x, sign1_y }, signSize, "Door"));

    // BonFire
    Math::vec2 bonfireSize = { 25.0, 25.0 };

    double bonfire1_y = platformY + (bonfireSize.y / 2.0);
    double bonfire2_y = platformY2 + (bonfireSize.y / 2.0);

    double bonfire1_x = 900.0;
    double bonfire2_x = 6000.0;
    double bonfire3_x = 7800.0;

    gom->Add(new Bonfire({ bonfire1_x, bonfire2_y }, bonfireSize));
    gom->Add(new Bonfire({ bonfire2_x, bonfire1_y }, bonfireSize));
    gom->Add(new Bonfire({ bonfire3_x, bonfire1_y }, bonfireSize));

    // Door
    Math::vec2 doorSize = { 80, 120 };
    double     doorY    = platformY + (doorSize.y / 2.0);

    gom->Add(new Door({ 8000.0, doorY }, doorSize));
    */
}

void Mode1::InitGame()
{
    auto gom = GetGSComponent<CS230::GameObjectManager>();

    worldTextManager = new WorldTextManager();
    worldTextManager->SetCamera(camera);
    AddGSComponent(worldTextManager);

    player = new Player({ 0.0, 800.0 });
    gom->Add(player);

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

    double platformY  = 200;
    double platformY2 = 500.0;

    // Sign
    Math::vec2 signSize = { 50.0, 25.0 };

    double sign1_y = platformY + (signSize.y / 2.0);
    double sign2_y = platformY2 + (signSize.y / 2.0);

    double sign1_x  = 0.0;
    double sign2_x  = 200.0;
    double sign3_x  = 800.0;
    double sign4_x  = 1100.0;
    double sign5_x  = 1900.0;
    double sign6_x  = 2700.0;
    double sign7_x  = 4500.0;
    double sign8_x  = 5000.0;
    double sign9_x  = 5900.0;
    double sign10_x = 7900.0;

    gom->Add(new Sign({ sign1_x, sign1_y }, signSize, "A/D to Move"));
    gom->Add(new Sign({ sign2_x, sign1_y }, signSize, "W or Space to Jump"));
    gom->Add(new Sign({ sign3_x, sign2_y }, signSize, "Press 'F' at Bonfire to Save"));
    gom->Add(new Sign({ sign4_x, sign1_y }, signSize, "Press 'R' to Respawn"));
    gom->Add(new Sign({ sign5_x, sign2_y }, signSize, "Press LShift to Dash"));
    gom->Add(new Sign({ sign6_x, sign1_y }, signSize, "Hold 'LShift' to Sprint"));
    gom->Add(new Sign({ sign7_x, sign1_y }, signSize, "Hold RMB: Shield (Reflects Light, Slows Move)"));
    gom->Add(new Sign({ sign8_x, sign1_y }, signSize, "Reflect Light to Hit the Star!"));
    gom->Add(new Sign({ sign9_x, sign1_y }, signSize, "Red Lasers Hurt! Parry with Timed Block! Be sure to Save at the Bonfire ahead!"));
    gom->Add(new Sign({ sign10_x, sign1_y }, signSize, "Door"));

    // BonFire
    Math::vec2 bonfireSize = { 25.0, 25.0 };

    double bonfire1_y = platformY + (bonfireSize.y / 2.0);
    double bonfire2_y = platformY2 + (bonfireSize.y / 2.0);

    double bonfire1_x = 900.0;
    double bonfire2_x = 6000.0;
    double bonfire3_x = 7800.0;

    gom->Add(new Bonfire({ bonfire1_x, bonfire2_y }, bonfireSize));
    gom->Add(new Bonfire({ bonfire2_x, bonfire1_y }, bonfireSize));
    gom->Add(new Bonfire({ bonfire3_x, bonfire1_y }, bonfireSize));

    // Door
    Math::vec2 doorSize = { 80, 120 };
    double     doorY    = platformY + (doorSize.y / 2.0);

    gom->Add(new Door({ 8000.0, doorY }, doorSize));
}

void Mode1::Update(double dt)
{
    UpdateGSComponents(dt);

    if (currentState == State::Loading)
    {
        if (mapManager->GetCurrentMap() && mapManager->GetCurrentMap()->IsLevelLoaded())
        {
            Engine::GetLogger().LogEvent("Mode1 Map Loading Complete! Starting Game...");
            InitGame();
            currentState = State::Playing;
        }
        return;
    }

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
        player->SetPosition({ 8000, 300 });
    }
}

void Mode1::Draw()
{
    CS200::IRenderer2D& renderer         = Engine::GetRenderer2D();
    Math::ivec2         display_size_int = Engine::GetWindow().GetSize();

    if (currentState == State::Loading)
    {
        Engine::GetWindow().Clear(CS200::BLACK);
        Math::TransformationMatrix screen_matrix = CS200::build_ndc_matrix(display_size_int);
        renderer.BeginScene(screen_matrix);

        CS230::Font& font       = Engine::GetFont(0);
        auto         loadingTex = font.PrintToTexture("Loading Tutorial...", CS200::WHITE);
        if (loadingTex)
        {
            Math::ivec2 texSize   = loadingTex->GetSize();
            Math::vec2  centerPos = { display_size_int.x * 0.5, display_size_int.y * 0.5 };
            Math::vec2  drawPos   = centerPos - Math::vec2{ texSize.x * 0.5, texSize.y * 0.5 };

            Math::TransformationMatrix transform = Math::TranslationMatrix(drawPos);
            loadingTex->Draw(transform);
        }
        renderer.EndScene();
        return;
    }

    Engine::GetWindow().Clear(CS200::BLACK);

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
#ifdef DEVELOPER_VERSION
    ImGui::Begin("Mode1 Debug");

    if (ImGui::CollapsingHeader("Global Info", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("FPS: %d", Engine::GetWindowEnvironment().FPS);
        if (camera)
        {
            Math::vec2 camPos = camera->GetPosition();
            ImGui::Text("Camera Pos: (%.1f, %.1f)", camPos.x, camPos.y);
        }
    }

    if (ImGui::CollapsingHeader("Object Inspector", ImGuiTreeNodeFlags_DefaultOpen))
    {
        auto gom = GetGSComponent<CS230::GameObjectManager>();
        if (gom)
        {
            gom->DrawAllImGui();
        }
    }

    ImGui::End();
#endif
}

void Mode1::Unload()
{
    ClearGSComponents();
    player           = nullptr;
    mapManager       = nullptr;
    worldTextManager = nullptr;
}
