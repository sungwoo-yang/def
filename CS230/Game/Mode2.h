/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#pragma once
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Vec2.h"
#include "../Engine/GameObjectManager.h"
#include "Ship.h"

class Mode2 : public CS230::GameState {
public:
    Mode2();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "Mode2";
    }

private:
    CS230::GameObjectManager gameobjectmanager;
};
