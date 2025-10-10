/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#include "../Engine/Engine.hpp"
#include "States.hpp"
#include "Mode1.hpp"
#include "Cat.hpp"
#include "Asteroid.hpp"
#include "Crates.hpp"
#include "Robot.hpp"
#include <iostream>

Mode1::Mode1() :
    cat_ptr(nullptr),
    camera({ { 0.15 * Engine::GetWindow().GetSize().x, 0 }, { 0.35 * Engine::GetWindow().GetSize().x, 0 } })
{
}

void Mode1::Load() {
    background.Add("Assets/Planets.png", 0.25);
    background.Add("Assets/Ships.png", 0.5);
    background.Add("Assets/Foreground.png", 1.0);

    camera.SetPosition({ 0, 0 });
    camera.SetLimit({ { 0, 0 }, background.GetSize() - Engine::GetWindow().GetSize() });
    cat_ptr = new Cat{ {300, floor}, camera };
    gameobjectmanager.Add(cat_ptr);
    gameobjectmanager.Add(new Asteroid({ 600, floor }));
    gameobjectmanager.Add(new Asteroid({ 1800, floor }));
    gameobjectmanager.Add(new Asteroid({ 2400, floor }));
    gameobjectmanager.Add(new Crates({ 900, floor }, 2));
    gameobjectmanager.Add(new Crates({ 1400, floor }, 1));
    gameobjectmanager.Add(new Crates({ 2000, floor }, 5));
    gameobjectmanager.Add(new Crates({ 4000, floor }, 3));
    gameobjectmanager.Add(new Crates({ 5600, floor }, 5));
    gameobjectmanager.Add(new Robot({ 1200, Mode1::floor }));
    gameobjectmanager.Add(new Robot({ 2200, Mode1::floor }));
    gameobjectmanager.Add(new Robot({ 3400, Mode1::floor }));
    gameobjectmanager.Add(new Robot({ 4200, Mode1::floor }));
}


void Mode1::Update(double dt) {
    camera.Update(cat_ptr->GetPosition());
    gameobjectmanager.UpdateAll(dt);

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R)) {
        Engine::GetGameStateManager().ReloadState();
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Enter)) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Mode2));
    }
}

void Mode1::Draw() {
    Engine::GetWindow().Clear(0x000000FF);

    Math::TransformationMatrix camera_matrix = camera.GetMatrix();

    background.Draw(camera);
    gameobjectmanager.DrawAll(camera.GetMatrix());
}



void Mode1::Unload() {
    background.Unload();
    gameobjectmanager.Unload();
    cat_ptr = nullptr;
}

