/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  TextureManager.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#pragma once
#include <filesystem>
#include <map>

namespace CS230 {
    class Texture;

    class TextureManager {
    public:
        Texture* Load(const std::filesystem::path& file_name);
        void Unload();

    private:
        std::map<std::filesystem::path, Texture*> textures;
    };
}
