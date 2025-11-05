/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Component.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 8, 2025
*/

#pragma once

namespace CS230 {
    class Component {
    public:
        virtual ~Component() {};
        virtual void Update([[maybe_unused]] double dt) {};
    };
}
