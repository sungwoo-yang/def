/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#include "../Engine/Engine.hpp"
#include "States.hpp"
#include "Mode2.hpp"
#include "ship.hpp"
#include "Meteor.hpp"

Mode2::Mode2()
{
}

void Mode2::Load() {
    gameobjectmanager.Add(new Ship({
        static_cast<double>(Engine::GetWindow().GetSize().x / 2),
        static_cast<double>(Engine::GetWindow().GetSize().y / 2)
        }));
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());
}

void Mode2::Update(double dt) {
    gameobjectmanager.UpdateAll(dt);

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R)) {
        Engine::GetGameStateManager().ReloadState();
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Enter)) {
        Engine::GetGameStateManager().ClearNextGameState();
    }
}

void Mode2::Draw() {
    Engine::GetWindow().Clear(0x000000FF);
    gameobjectmanager.DrawAll(Math::TransformationMatrix());
}

void Mode2::Unload() {
    gameobjectmanager.Unload();
}