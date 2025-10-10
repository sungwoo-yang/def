/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 20, 2025
*/

#include "Ship.h"
#include "../Engine/Engine.h"

Ship::Ship(Math::vec2 start_position) :
    GameObject(start_position, 0.0, { scale, scale })
{
    sprite.Load("Assets/Ship.spt");
    flame_left.Load("Assets/Flame.spt");
    flame_right.Load("Assets/Flame.spt");

    flame_left.PlayAnimation(static_cast<int>(Animations::Off));
    flame_right.PlayAnimation(static_cast<int>(Animations::Off));

    size = sprite.GetFrameSize();
    flame_on = false;
}

void Ship::Update(double dt) {
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::W)) {
        UpdateVelocity(Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, speed * dt });
        if (flame_on == false) {
            flame_left.PlayAnimation(static_cast<int>(Animations::On));
            flame_right.PlayAnimation(static_cast<int>(Animations::On));
            flame_on = true;
        }
    }
    else {
        flame_left.PlayAnimation(static_cast<int>(Animations::Off));
        flame_right.PlayAnimation(static_cast<int>(Animations::Off));
        flame_on = false;
    }
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::A)) {
        UpdateRotation(rotation_speed * dt);
    }
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::D)) {
        UpdateRotation(-rotation_speed * dt);
    }

    UpdateVelocity(-GetVelocity() * drag * dt);

    Engine::GetLogger().LogDebug("Velocity: " + std::to_string(GetVelocity().x) + ", " + std::to_string(GetVelocity().y));

    UpdatePosition({ GetVelocity().x * dt, GetVelocity().y * dt });

    double halfwidth = size.x / 2;
    double halfheight = size.y / 2;

    if (GetPosition().y - halfheight > GetScreenHeight()) {
        SetPosition({ GetPosition().x, -halfheight });
    }
    else if (GetPosition().x + halfwidth < 0) {
        SetPosition({ GetScreenWidth() + halfwidth, GetPosition().y });
    }
    else if (GetPosition().y + halfheight < 0) {
        SetPosition({ GetPosition().x, GetScreenHeight() + halfheight});
    }
    else if (GetPosition().x - halfwidth > GetScreenWidth()) {
        SetPosition({ -halfwidth, GetPosition().y });
    }

    flame_left.Update(dt);
    flame_right.Update(dt);
}

void Ship::Draw(Math::TransformationMatrix camera_matrix) {
    flame_left.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(sprite.GetHotSpot(1)));
    flame_right.Draw(camera_matrix * GetMatrix() * Math::TranslationMatrix(sprite.GetHotSpot(2)));
    sprite.Draw(camera_matrix * GetMatrix());
}
