/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Engine.h
Project:    CS230 Engine
Author:     Sungwoo Yang
Created:    March 11, 2025
*/

#pragma once

#include "Logger.h"
#include "Window.h"
#include "GameStateManager.h"
#include "TextureManager.h"
#include "Input.h"
#include <chrono>

class Engine {
public:
    static Engine& Instance() {
        static Engine instance;
        return instance;
    }

    static CS230::Logger& GetLogger() {
        return Instance().logger;
    }

    static CS230::Window& GetWindow() {
        return Instance().window;
    }

    static CS230::GameStateManager& GetGameStateManager() {
        return Instance().gamestatemanager;
    }

    static CS230::Input& GetInput() {
        return Instance().input;
    }

    void Start(std::string window_title);
    void Stop();
    void Update();
    
    bool HasGameEnded();

    static CS230::TextureManager& GetTextureManager();
private:
    Engine();

    std::chrono::system_clock::time_point last_tick;
    std::chrono::system_clock::time_point last_test;
    std::chrono::system_clock::time_point now;

    int frame_count = 0;

    static constexpr double TargetFPS = 30.0;
    static constexpr int FPSDuration = 5;
    static constexpr int FPSTargetFrames = static_cast<int>(FPSDuration * TargetFPS);

    CS230::Logger logger;
    CS230::Window window;
    CS230::GameStateManager gamestatemanager;
    CS230::Input input;
    inline static CS230::TextureManager texture_manager;
};

