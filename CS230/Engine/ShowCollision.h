/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  ShowCollision.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    May 13, 2025
*/

#pragma once
#include "Component.h"

namespace CS230 {
    class ShowCollision : public CS230::Component {
    public:
        ShowCollision();
        void Update(double dt) override;
        bool Enabled();
    private:
        bool enabled;
    };
}
