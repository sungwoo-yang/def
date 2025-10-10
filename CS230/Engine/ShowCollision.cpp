/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ShowCollision.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 13, 2025
*/

#include "ShowCollision.h"
#include "Engine.h"
#include "Input.h"

CS230::ShowCollision::ShowCollision() : enabled(false) {}

void CS230::ShowCollision::Update([[maybe_unused]]double dt) {
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Tab) == true) {
        enabled = !enabled;
    }
}

bool CS230::ShowCollision::Enabled() {
    return enabled;
}
