/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Texture.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 18, 2025
*/

#pragma once
#include "Vec2.h"
#include "Matrix.h"
#include <string>
#include <filesystem>
#include <raylib.h>

namespace CS230 {
    class TextureManager;

    class Texture {
        friend class TextureManager;

    public:
        Texture();
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&& temporary) noexcept;
        Texture& operator=(Texture&& temporary) noexcept;

        //void Load(const std::filesystem::path& file_name);
        void Draw(Math::TransformationMatrix display_matrix);
        void Draw(Math::TransformationMatrix display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size);
        Math::ivec2 GetSize() const;

    private:
        Texture(const std::filesystem::path& file_name);
        Texture2D texture;
        unsigned int texture_id = 0;
        std::string texture_name;
    };
}
