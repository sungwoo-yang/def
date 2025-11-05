/*
Copyright (C) 2025 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameStateManager.cpp
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#include "GameStateManager.h"
#include "Engine.h"

CS230::GameStateManager::GameStateManager() :
    current_gamestate(nullptr),
    next_gamestate(nullptr),
    status(Status::STARTING)
{
}

void CS230::GameStateManager::AddGameState(GameState& gamestate) {
    gamestates.push_back(&gamestate);
}

void CS230::GameStateManager::SetNextGameState(int index) {
    next_gamestate = gamestates[index];
}

void CS230::GameStateManager::ReloadState() {
    status = Status::UNLOADING;
}

void CS230::GameStateManager::ClearNextGameState() {
    next_gamestate = nullptr;
}

bool CS230::GameStateManager::HasGameEnded() {
    return status == Status::EXIT;
}

void CS230::GameStateManager::Update(double dt) {
    switch (status) {
    case Status::STARTING:
        if (gamestates.empty())
        {
            status = Status::STOPPING;
        }
        else
        {
            next_gamestate = gamestates[0];
            status = Status::LOADING;
        }
        break;
    case Status::LOADING:
        current_gamestate = next_gamestate;
        Engine::GetLogger().LogEvent("Load " + current_gamestate->GetName());
        current_gamestate->Load();
        Engine::GetLogger().LogEvent("Load Complete");
        status = Status::UPDATING;
        break;
    case Status::UPDATING:
        if (current_gamestate != next_gamestate)
        {
            status = Status::UNLOADING;
        }
        else
        {
            Engine::GetLogger().LogVerbose("Update " + current_gamestate->GetName());
            current_gamestate->Update(dt);

            if (current_gamestate->GetGSComponent<GameObjectManager>() != nullptr) {
                current_gamestate->GetGSComponent<GameObjectManager>()->CollisionTest();
            }

            current_gamestate->Draw();
        }
        break;
    case Status::UNLOADING:
        Engine::GetLogger().LogEvent("Unloading " + current_gamestate->GetName());
        current_gamestate->Unload();
        Engine::GetTextureManager().Unload();
        Engine::GetLogger().LogEvent("Unload Complete.");
        if (next_gamestate != nullptr)
        {
            status = Status::LOADING;
        }
        else
        {
            status = Status::STOPPING;
        }
        break;
    case Status::STOPPING:
        status = Status::EXIT;
        break;
    case Status::EXIT:
        break;
    }
}

