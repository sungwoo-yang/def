#pragma once
#include "Engine/Camera.hpp"
#include "Engine/GameState.hpp"

// Mode1 ���� ����
class Mode1 : public CS230::GameState
{
public:
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw()   override;

    void DrawImGui() override
    {
    }

    gsl::czstring GetName() const override
    {
        return "Mode1";
    }

private:
    CS230::Camera* camera;
    Math::rect     level1_boundary = {
        {  700.f,     0.f },
        { 4500.f, -4500.f }
    };
};

#pragma once
