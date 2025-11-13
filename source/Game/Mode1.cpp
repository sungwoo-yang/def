#include "Game/Mode1.h"
#include "Engine/Engine.hpp"
#include "Engine/GameObjectManager.hpp"
#include "Engine/MapManager.h"
#include "Engine/ShowCollision.hpp"
#include "Engine/Input.hpp"
#include "CS200/NDC.hpp"
#include "CS200/IRenderer2D.hpp"
#include "Engine/Window.hpp"

void Mode1::Load()
{
    AddGSComponent(new CS230::GameObjectManager());

    AddGSComponent(new CS230::ShowCollision());

    camera = new CS230::Camera(Math::rect{
        {   0,   0 },
        { 800, 600 }
    });      

    camera->SetPosition({ 1500, -2000 }); 
    AddGSComponent(camera);

    CS230::MapManager* mapManager = new CS230::MapManager();

    mapManager->AddMap(new CS230::Map("Assets/maps/Main_level.svg"));

    mapManager->LoadMap();
    AddGSComponent(mapManager);
}

void Mode1::Update(double dt)
{
    UpdateGSComponents(dt);
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);

    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left))
    {
        camera->SetPosition(camera->GetPosition() + Math::vec2{ -300 * dt, 0 });
    }
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right))
    {
        camera->SetPosition(camera->GetPosition() + Math::vec2{ 300 * dt, 0 });
    }
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Up))
    {
        camera->SetPosition(camera->GetPosition() + Math::vec2{ 0, 300 * dt });
    }
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Down))
    {
        camera->SetPosition(camera->GetPosition() + Math::vec2{ 0, -300 * dt });
    }

    GetGSComponent<CS230::GameObjectManager>()->CollisionTest();
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
}

void Mode1::Unload()
{
    ClearGSComponents();
}


