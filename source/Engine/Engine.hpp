/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#pragma once

#include <gsl/gsl>
#include <string_view>

namespace CS230
{
    class Logger;
    class Window;
    class Input;
}

class Engine
{
public:
    static Engine& Instance();

    static CS230::Logger& GetLogger();

    static CS230::Window& GetWindow();

    static CS230::Input& GetInput();

public:
    void Start(std::string_view window_title);
    void Stop();
    void Update();

    bool HasGameEnded();

private:
    Engine();
    ~Engine();

    class Impl;
    gsl::owner<Impl*> impl = nullptr;
};
