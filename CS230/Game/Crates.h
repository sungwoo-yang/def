/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Crates.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#pragma once
#include "../Engine/GameObject.h"

class Crates : public CS230::GameObject {
public:
    Crates(Math::vec2 position, int size);
};
