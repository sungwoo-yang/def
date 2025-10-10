/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#pragma once
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Vec2.h"
#include "../Engine/Matrix.h"
#include "../Engine/GameObjectManager.h"
#include "Background.h"

class Cat;

class Mode1 : public CS230::GameState {
public:
    Mode1();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "Mode1";
    }

    static constexpr double gravity = 800;
    static constexpr double floor = 80;

private:
    CS230::GameObjectManager gameobjectmanager;
    Cat* cat_ptr;
    CS230::Camera camera;
    Background background;
};
