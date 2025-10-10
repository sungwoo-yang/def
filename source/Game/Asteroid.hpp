/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 24, 2025
*/

#pragma once
#include "..\Engine\GameObject.hpp"

class Asteroid : public CS230::GameObject {
public:
    Asteroid(Math::vec2 start_position);

private:
    static constexpr double bounce_velocity = 500;

    enum class Animations {
        Land,
        None
    };

    class State_Bouncing : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Bouncing"; }
    };
    
    State_Bouncing state_bouncing;

    class State_Landing : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Bouncing"; }
    };

    State_Landing state_land;
};
