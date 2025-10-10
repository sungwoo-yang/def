/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Background.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 3, 2025
*/

#pragma once
#include "../Engine/Texture.hpp"
#include "../Engine/Camera.hpp"

class Background {
public:
    void Add(const std::filesystem::path& texture_path, double speed);
    void Unload();
    void Draw(const CS230::Camera& camera);
    Math::ivec2 GetSize();
private:
    struct ParallaxLayer {
        CS230::Texture* texture = nullptr;
        double speed = 1;
    };

    std::vector<ParallaxLayer> backgrounds;
};
