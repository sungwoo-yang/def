#pragma once
#include "Engine/Camera.hpp"
#include "Engine/GameState.hpp"
#include "Game/MiniMap.h"

class Player;

namespace CS230
{
    class MapManager;
}

class Mode1 : public CS230::GameState
{
public:
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;
    void DrawImGui() override;

    gsl::czstring GetName() const override
    {
        return "Mode1";
    }

private:
    CS230::Camera*     camera;
    Player*            player;
    CS230::MapManager* mapManager;

    Math::rect level1_boundary = {
        {  -3000.f,     -2000.f },
        { 3000.f, 2000.f }
    };

    MiniMap minimap;
};