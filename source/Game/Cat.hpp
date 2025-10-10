/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 20, 2025
*/

#pragma once
#include "../Engine/Input.hpp"
#include "../Engine/Camera.hpp"
#include "../Engine/Matrix.hpp"
#include "../Engine/GameObject.hpp"

class Cat : public CS230::GameObject {
public:
    Cat(Math::vec2 start_position, const CS230::Camera& camera);
    void Update(double dt) override;
    const Math::vec2& GetPosition() const { return GameObject::GetPosition(); };

private:
    static constexpr double x_acceleration = 400;
    static constexpr double x_drag = 450;
    static constexpr double max_velocity = 300;
    static constexpr double jump_velocity = 650;
    const CS230::Camera& camera;
    void update_x_velocity(double dt);

    enum class Animations {
        Idle,
        Running,
        Jumping,
        Falling,
        Skidding
    };

    //class State {
    //public:
    //    virtual void Enter(Cat* cat) = 0;
    //    virtual void Update(Cat* cat, double dt) = 0;
    //    virtual void CheckExit(Cat* cat) = 0;
    //    virtual std::string GetName() = 0;
    //};

    class State_Jumping : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Jumping"; }
    };


    State_Jumping state_jumping;

    //void change_state(State* new_state);
    //State* current_state = nullptr;

    class State_Idle : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Idle"; }
    };

    State_Idle state_idle;

    class State_Falling : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Falling"; }
    };

    State_Falling state_falling;

    class State_Running : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Running"; }
    };

    State_Running state_running;

    class State_Skidding : public State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Skidding"; }
    };

    State_Skidding state_skidding;
};
