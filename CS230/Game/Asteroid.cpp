/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 24, 2025
*/

#include "Asteroid.h"
#include "Mode1.h"
#include "../Engine/Engine.h"

Asteroid::Asteroid(Math::vec2 start_position) :
	GameObject(start_position)
{
	sprite.Load("Assets/Asteroid.spt");
	change_state(&state_land);
}

void Asteroid::State_Bouncing::Enter(GameObject* object) {
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->SetVelocity({ 0, bounce_velocity });
	asteroid->sprite.PlayAnimation(static_cast<int>(Animations::None));
}
void Asteroid::State_Bouncing::Update(GameObject* object, double dt) {
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->UpdateVelocity({0, -Mode1::gravity * dt});
}
void Asteroid::State_Bouncing::CheckExit(GameObject* object) {
	Asteroid* asteroid = static_cast<Asteroid*>(object);
    if(asteroid->GetPosition().y < Mode1::floor) {
        asteroid->SetPosition({ asteroid->GetPosition().x, Mode1::floor });
        asteroid->SetVelocity({ 0, 0 });
        asteroid->change_state(&asteroid->state_land);
    }
}

void Asteroid::State_Landing::Enter(GameObject* object) {
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->sprite.PlayAnimation(static_cast<int>(Animations::Land));
}
void Asteroid::State_Landing::Update([[maybe_unused]] GameObject* object, [[maybe_unused]] double dt) {
}
void Asteroid::State_Landing::CheckExit(GameObject* object) {
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	if (asteroid->sprite.AnimationEnded()) {
		asteroid->change_state(&asteroid->state_bouncing);
	}
}