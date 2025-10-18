/**
 * \file
 * \author Rudy Castan
 * \author Jonathan Holmes
 * \date 2025 Fall
 * \par CS200 Computer Graphics I
 * \copyright DigiPen Institute of Technology
 */
#include "Engine.hpp"
#include "Input.hpp"
#include "Logger.hpp"
#include "Window.hpp"
#include <chrono>

// Pimpl implementation class
class Engine::Impl
{
public:
    Impl()
        :
#ifdef DEVELOPER_VERSION
          logger(CS230::Logger::Severity::Debug, true, std::chrono::system_clock::now())
#else
          logger(CS230::Logger::Severity::Event, false, std::chrono::system_clock::now())
#endif
          ,
          window{}, input{}
    {
    }

    CS230::Logger logger;
    CS230::Window window;
    CS230::Input  input;
};

Engine& Engine::Instance()
{
    static Engine instance;
    return instance;
}

CS230::Logger& Engine::GetLogger()
{
    return Instance().impl->logger;
}

CS230::Window& Engine::GetWindow()
{
    return Instance().impl->window;
}

CS230::Input& Engine::GetInput()
{
    return Instance().impl->input;
}

void Engine::Start(std::string_view window_title)
{
    impl->logger.LogEvent("Engine Started");
#if defined(DEVELOPER_VERSION)
    impl->logger.LogEvent("Developer Build");
#endif
    impl->window.Start(window_title);
}

void Engine::Stop()
{
    impl->logger.LogEvent("Engine Stopped");
}

void Engine::Update()
{
    impl->window.Update();
    impl->input.Update();
}

bool Engine::HasGameEnded()
{
    return impl->window.IsClosed();
}

Engine::Engine() : impl(new Impl())
{
}

Engine::~Engine()
{
    delete impl;
}
