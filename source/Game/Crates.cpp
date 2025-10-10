/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Crates.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 22, 2025
*/

#include "Crates.hpp"

Crates::Crates(Math::vec2 position, int size) : CS230::GameObject(position) {
    switch (size) {
    case 1:
        sprite.Load("Assets/Crates1.spt");
        break;
    case 2:
        sprite.Load("Assets/Crates2.spt");
        break;
    case 3:
        sprite.Load("Assets/Crates3.spt");
        break;
    case 5:
        sprite.Load("Assets/Crates5.spt");
        break;
    }
}
