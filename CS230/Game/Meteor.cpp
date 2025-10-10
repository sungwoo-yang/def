/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#include "Meteor.h"
#include "../Engine/Engine.h"

Meteor::Meteor() : GameObject({0, 0}) {
	sprite.Load("Assets/Meteor.spt");

	double random_vx = (static_cast<double>(rand()) / RAND_MAX) * default_velocity * 2 - default_velocity;
	double random_vy = (static_cast<double>(rand()) / RAND_MAX) * default_velocity * 2 - default_velocity;
	SetVelocity({ random_vx, random_vy });

	double pi = 3.14;
	double random_angle = (static_cast<double>(rand()) / RAND_MAX) * pi * 2;
	SetRotation(random_angle);

	double random_x = (static_cast<double>(rand()) / RAND_MAX) * Engine::GetWindow().GetSize().x;
	double random_y = (static_cast<double>(rand()) / RAND_MAX) * Engine::GetWindow().GetSize().y;
	SetPosition({ random_x, random_y });
}

void Meteor::Update(double dt) {
	GameObject::Update(dt);

	Math::vec2 pos = GetPosition();
	Math::ivec2 screen = Engine::GetWindow().GetSize();

	double half_width = sprite.GetFrameSize().x / 2;
	double half_height = sprite.GetFrameSize().y / 2;

	if (pos.x + half_width < 0) {
		pos.x = screen.x + half_width;
	}

	else if (pos.x - half_width > screen.x) {
		pos.x = -half_width;
	}

	if (pos.y + half_height < 0) {
		pos.y = screen.y + half_height;
	}

	else if (pos.y - half_height > screen.y) {
		pos.y = -half_height;
	}

	SetPosition(pos);
}