/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Texture.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 18, 2025
*/

#include "Texture.h"
#include "Engine.h"

CS230::Texture::Texture() {
}

CS230::Texture::~Texture() {
    UnloadTexture(texture);
}

CS230::Texture::Texture(Texture&& temporary) noexcept : texture(temporary.texture) {
    temporary.texture.id = 0;
    temporary.texture.width = 0;
    temporary.texture.height = 0;
    temporary.texture.mipmaps = 0;
    temporary.texture.format = 0;
}

CS230::Texture& CS230::Texture::operator=(Texture&& temporary) noexcept {
    std::swap(texture.id, temporary.texture.id);
    std::swap(texture.width, temporary.texture.width);
    std::swap(texture.height, temporary.texture.height);
    std::swap(texture.mipmaps, temporary.texture.mipmaps);
    std::swap(texture.format, temporary.texture.format);
    return *this;
}

//void CS230::Texture::Load(const std::filesystem::path& file_name) {
//    texture = LoadTexture(file_name.string().c_str());
//    SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);
//}

Math::ivec2 CS230::Texture::GetSize() const {
    return { texture.width, texture.height };
}

void CS230::Texture::Draw(Math::TransformationMatrix display_matrix) {
    Math::vec2 bottom_left = display_matrix * Math::vec2{ 0, 0 };
    Math::vec2 bottom_right = display_matrix * Math::vec2{ double(texture.width), 0 };
    Math::vec2 top_left = display_matrix * Math::vec2{ 0, double(texture.height) };
    Math::vec2 top_right = display_matrix * Math::vec2{ double(texture.width), double(texture.height) };

    const double H = Engine::GetWindow().GetSize().y;
    bottom_left.y = bottom_left.y * -1 + H;
    bottom_right.y = bottom_right.y * -1 + H;
    top_left.y = top_left.y * -1 + H;
    top_right.y = top_right.y * -1 + H;

    constexpr Color tint = WHITE;

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);
    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlTexCoord2f(0, 0);
    rlVertex2f(float(top_left.x), float(top_left.y));
    rlTexCoord2f(0, 1);
    rlVertex2f(float(bottom_left.x), float(bottom_left.y));
    rlTexCoord2f(1, 1);
    rlVertex2f(float(bottom_right.x), float(bottom_right.y));
    rlTexCoord2f(1, 0);
    rlVertex2f(float(top_right.x), float(top_right.y));

    rlEnd();
    rlSetTexture(0);
}

void CS230::Texture::Draw(Math::TransformationMatrix display_matrix, Math::ivec2 texel_position, Math::ivec2 frame_size) {
    Math::vec2 bottom_left = display_matrix * Math::vec2{ 0, 0 };
    Math::vec2 bottom_right = display_matrix * Math::vec2{ double(frame_size.x), 0 };
    Math::vec2 top_left = display_matrix * Math::vec2{ 0, double(frame_size.y) };
    Math::vec2 top_right = display_matrix * Math::vec2{ double(frame_size.x), double(frame_size.y) };

    const double H = Engine::GetWindow().GetSize().y;
    bottom_left.y = bottom_left.y * -1 + H;
    bottom_right.y = bottom_right.y * -1 + H;
    top_left.y = top_left.y * -1 + H;
    top_right.y = top_right.y * -1 + H;

    const float left_u = float(texel_position.x) / float(texture.width);
    const float right_u = (float(texel_position.x) + frame_size.x) / float(texture.width);
    const float bottom_v = (float(texel_position.y) + frame_size.y) / float(texture.height);
    const float top_v = float(texel_position.y) / float(texture.height);

    constexpr Color tint = WHITE;

    rlSetTexture(texture.id);
    rlBegin(RL_QUADS);

    rlColor4ub(tint.r, tint.g, tint.b, tint.a);
    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlTexCoord2f(left_u, top_v);
    rlVertex2f(float(top_left.x), float(top_left.y));
    rlTexCoord2f(left_u, bottom_v);
    rlVertex2f(float(bottom_left.x), float(bottom_left.y));
    rlTexCoord2f(right_u, bottom_v);
    rlVertex2f(float(bottom_right.x), float(bottom_right.y));
    rlTexCoord2f(right_u, top_v);
    rlVertex2f(float(top_right.x), float(top_right.y));

    rlEnd();
    rlSetTexture(0);
}

CS230::Texture::Texture(const std::filesystem::path& file_name) : texture_name(file_name.string()) {
    texture = LoadTexture(file_name.string().c_str());
    SetTextureWrap(texture, TEXTURE_WRAP_CLAMP);
}