/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameObject.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    April 17, 2025
*/

#pragma once
#include "Sprite.h"

namespace Math { class TransformationMatrix; }

namespace CS230 {
    class GameObject {
    public:
        GameObject(Math::vec2 position);
        GameObject(Math::vec2 position, double rotation, Math::vec2 scale);
        virtual ~GameObject() {}

        virtual void Update(double dt);
        virtual void Draw(Math::TransformationMatrix camera_matrix);

        const Math::TransformationMatrix& GetMatrix();
        const Math::vec2& GetPosition() const;
        const Math::vec2& GetVelocity() const;
        const Math::vec2& GetScale() const;
        double GetRotation() const;

    protected:
        void SetPosition(Math::vec2 new_position);
        void UpdatePosition(Math::vec2 delta);
        void SetVelocity(Math::vec2 new_position);
        void UpdateVelocity(Math::vec2 delta);
        void SetScale(Math::vec2 new_scale);
        void UpdateScale(Math::vec2 delta);
        void SetRotation(double new_rotation);
        void UpdateRotation(double delta);

        Sprite sprite;

        class State {
        public:
            virtual void Enter(GameObject* object) = 0;
            virtual void Update(GameObject* object, double dt) = 0;
            virtual void CheckExit(GameObject* object) = 0;
            virtual std::string GetName() = 0;
        };
        State* current_state;
        void change_state(State* new_state);

    private:
        Math::TransformationMatrix object_matrix;
        bool matrix_outdated = true;

        double rotation;
        Math::vec2 scale;
        Math::vec2 position;
        Math::vec2 velocity;

        class State_None : public State {
        public:
            void Enter(GameObject*) override {}
            void Update(GameObject*, double) override {}
            void CheckExit(GameObject*) override {}
            std::string GetName() { return ""; }
        };

        State_None state_none;
    };
}


