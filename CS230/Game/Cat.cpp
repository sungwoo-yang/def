/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 20, 2025
*/

#include "Cat.h"
#include "../Engine/Engine.h"
#include "Mode1.h"

Cat::Cat(Math::vec2 start_position, const CS230::Camera& camera) :
    GameObject(start_position),
    camera(camera)
    //current_state(&state_idle)
{
    sprite.Load("Assets/Cat.spt");
    change_state(&state_idle);
    //current_state->Enter(this);
}

void Cat::Update(double dt) {
    GameObject::Update(dt);
    // Boundary Check
    if (GetPosition().x < camera.GetPosition().x + sprite.GetFrameSize().x / 2) {
        SetPosition({ camera.GetPosition().x + sprite.GetFrameSize().x / 2, GetPosition().y });
        SetVelocity({ 0, GetVelocity().y });
    }
    if (GetPosition().x + sprite.GetFrameSize().x / 2 > camera.GetPosition().x + Engine::GetWindow().GetSize().x) {
        SetPosition({ camera.GetPosition().x + Engine::GetWindow().GetSize().x - sprite.GetFrameSize().x / 2, GetPosition().y });
        SetVelocity({ 0, GetVelocity().y });
    }
}

void Cat::update_x_velocity(double dt) {
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        UpdateVelocity({ x_acceleration * dt, 0 });
        if (GetVelocity().x > max_velocity) {
            SetVelocity({ max_velocity, GetVelocity().y });
        }
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        UpdateVelocity({ -x_acceleration * dt, 0 });
        if (GetVelocity().x < -max_velocity) {
            SetVelocity({ -max_velocity, GetVelocity().y });
        }
    }
    else {
        if (GetVelocity().x > x_drag * dt) {
            UpdateVelocity({ -x_drag * dt, 0 });
        }
        else if (GetVelocity().x < -x_drag * dt) {
            UpdateVelocity({ x_drag * dt, 0 });
        }
        else {
            SetVelocity({0, GetVelocity().y});
        }
    }
}

//void Cat::change_state(State* new_state) {
//    Engine::GetLogger().LogDebug("Cat Leaving State: " + current_state->GetName());
//    Engine::GetLogger().LogDebug("Cat Entering State: " + new_state->GetName());
//    current_state = new_state;
//    current_state->Enter(this);
//}

// Jumping
void Cat::State_Jumping::Enter(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    cat->SetVelocity({ cat->GetVelocity().x, Cat::jump_velocity });
    cat->sprite.PlayAnimation(static_cast<int>(Animations::Jumping));
}
void Cat::State_Jumping::Update(GameObject* object, double dt) {
    Cat* cat = static_cast<Cat*>(object);
    cat->UpdateVelocity({ 0, -Mode1::gravity * dt });
    cat->update_x_velocity(dt);
}
void Cat::State_Jumping::CheckExit(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Up) == false) {
        cat->change_state(&cat->state_falling);
        cat->SetVelocity({ cat->GetVelocity().x, 0 });
    }
    else if (cat->GetVelocity().y <= 0) {
        cat->change_state(&cat->state_falling);
    }
}

//Idle
void Cat::State_Idle::Enter(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    cat->sprite.PlayAnimation(static_cast<int>(Animations::Idle));
}
void Cat::State_Idle::Update([[maybe_unused]] GameObject* object, [[maybe_unused]]double dt) {
}
void Cat::State_Idle::CheckExit(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        cat->change_state(&cat->state_running);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
        cat->change_state(&cat->state_jumping);
    }
}

//Falling
void Cat::State_Falling::Enter(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    cat->sprite.PlayAnimation(static_cast<int>(Animations::Falling));
}
void Cat::State_Falling::Update(GameObject* object, double dt) {
    Cat* cat = static_cast<Cat*>(object);
    cat->UpdateVelocity({ 0, -Mode1::gravity * dt });
    cat->update_x_velocity(dt);
}
void Cat::State_Falling::CheckExit(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetPosition().y <= Mode1::floor) {
        cat->SetVelocity({ cat->GetVelocity().x, 0});
        cat->SetPosition({ cat->GetPosition().x, Mode1::floor });
        if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left) 
            && Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
            cat->change_state(&cat->state_running);
        }
        else if (cat->GetVelocity().y != 0) {
            cat->change_state(&cat->state_skidding);
        }
        else
        {
            cat->change_state(&cat->state_idle);
        }
    }
}   

//Running
void Cat::State_Running::Enter(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        cat->SetScale({ -1, 1 });
    }
    else if (Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        cat->SetScale({ 1, 1 });
    }
    cat->sprite.PlayAnimation(static_cast<int>(Animations::Running));
}
void Cat::State_Running::Update(GameObject* object, double dt) {
    Cat* cat = static_cast<Cat*>(object);
    cat->update_x_velocity(dt);
}
void Cat::State_Running::CheckExit(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetVelocity().x == 0) {
        cat->change_state(&cat->state_idle);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
        cat->change_state(&cat->state_jumping);
    }
    else if (cat->GetVelocity().x > 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left)
        || cat->GetVelocity().x < 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        cat->change_state(&cat->state_skidding);
    }
}

//Skidding
void Cat::State_Skidding::Enter(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    cat->sprite.PlayAnimation(static_cast<int>(Animations::Skidding));
}
void Cat::State_Skidding::Update(GameObject* object, double dt) {
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetVelocity().x > 0) {
        cat->UpdateVelocity({ -(x_drag + x_acceleration) * dt, 0 });
        if (cat->GetVelocity().x < 0) {
            cat->SetVelocity({ 0, cat->GetVelocity().y });
        }
    }
    else if (cat->GetVelocity().x < 0) {
        cat->UpdateVelocity({ (x_drag + x_acceleration) * dt, 0 });
        if (cat->GetVelocity().x > 0) {
            cat->SetVelocity({ 0, cat->GetVelocity().y });
        }
    }
}
void Cat::State_Skidding::CheckExit(GameObject* object) {
    Cat* cat = static_cast<Cat*>(object);
    if (cat->GetVelocity().x == 0) {
        cat->change_state(&cat->state_idle);
    }
    else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
        cat->change_state(&cat->state_jumping);
    }
    else if (cat->GetVelocity().x < 0 &&
        Engine::GetInput().KeyDown(CS230::Input::Keys::Left) &&
        !Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
        cat->change_state(&cat->state_running);
    }   
    else if (cat->GetVelocity().x > 0 &&
        Engine::GetInput().KeyDown(CS230::Input::Keys::Right) &&
        !Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) {
        cat->change_state(&cat->state_running);
    }
}