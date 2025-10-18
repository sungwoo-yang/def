/**
 * \file
 * \author Rudy Castan
 * \date 2024 Fall
 * \copyright DigiPen Institute of Technology
 */

#include "CS200/ImGuiHelper.hpp"
#include "Engine/Engine.hpp"
#include "Engine/Error.hpp"
#include "Engine/Input.hpp"
#include "Engine/Path.hpp"
#include "Engine/Window.hpp"
#include <GL/glew.h>
#include <SDL.h>
#include <al.h>
#include <alc.h>
#include <deque>
#include <gsl/gsl>
#include <imgui.h>
#include <iostream>
#include <stb_image.h>
#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>

namespace
{
    [[maybe_unused]] int  gWindowWidth  = 400;
    [[maybe_unused]] int  gWindowHeight = 400;
    [[maybe_unused]] bool gNeedResize   = false;

    class Demo
    {
    public:
        void Setup();
        void Shutdown();
        void Draw() const;
        void ImGuiDraw();

    private:
        CS200::RGBA background_color{ 0x6495edff }; // https://www.colorhexa.com/6495ed

        struct
        {
            GLuint handle = 0;
            int    width  = 0;
            int    height = 0;
            bool   loaded = false;
        } example_image;

        ALuint                  alBufferHandles[2] = { 0 };
        ALuint                  alSourceHandles[2] = { 0 };
        gsl::owner<ALCdevice*>  al_device          = nullptr;
        gsl::owner<ALCcontext*> al_context         = nullptr;
        ImGuiHelper::Viewport   viewport{};

        // Keyboard event history and highlighting
        struct KeyEvent
        {
            CS230::Input::Keys key;
            std::string        action;
            double             timeStamp;
        };

        std::deque<KeyEvent>                 keyEventHistory;
        std::map<CS230::Input::Keys, double> keyHighlightTimer;
        static constexpr double              HIGHLIGHT_DURATION = 1.0; // 1 second highlight
    };

    Demo demo;
}

#if defined(__EMSCRIPTEN__)
#    include <emscripten.h>
#    include <emscripten/bind.h>
#    include <emscripten/em_asm.h>

void main_loop()
{
    Engine& engine = Engine::Instance();
    if (gNeedResize)
    {
        Engine::GetWindow().ForceResize(gWindowWidth, gWindowHeight);
        gNeedResize = false;
    }

    demo.Draw();
    demo.ImGuiDraw();
    engine.Update();

    if (engine.HasGameEnded())
    {
        emscripten_cancel_main_loop();
        engine.Stop();
    }
}

EMSCRIPTEN_BINDINGS(main_window)
{
    emscripten::function(
        "setWindowSize", emscripten::optional_override(
                             [](int sizeX, int sizeY)
                             {
                                 sizeX                  = (sizeX < 400) ? 400 : sizeX;
                                 sizeY                  = (sizeY < 400) ? 400 : sizeY;
                                 const auto window_size = Engine::GetWindow().GetSize();
                                 if (sizeX != window_size.x || sizeY != window_size.y)
                                 {
                                     gNeedResize   = true;
                                     gWindowWidth  = sizeX;
                                     gWindowHeight = sizeY;
                                 }
                             }));
}
#endif

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
    try
    {
        Engine& engine = Engine::Instance();
        engine.Start("TODO CS200 HW1"); // TODO put your full name here
        demo.Setup();

#if !defined(__EMSCRIPTEN__)
        while (engine.HasGameEnded() == false)
        {
            demo.Draw();
            demo.ImGuiDraw();
            engine.Update();
        }
        engine.Stop();
#else
        // https://emscripten.org/docs/api_reference/emscripten.h.html#c.emscripten_set_main_loop
        constexpr bool simulate_infinite_loop  = true;
        constexpr int  match_browser_framerate = -1;
        emscripten_set_main_loop(main_loop, match_browser_framerate, simulate_infinite_loop);
#endif

        demo.Shutdown();
        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return -1;
    }
}

namespace
{
    // https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
    bool LoadTextureFromFile(const std::filesystem::path& filename, GLuint& out_texture, int& out_width, int& out_height)
    {
        // Load from file
        int            image_width  = 0;
        int            image_height = 0;
        unsigned char* image_data   = stbi_load(filename.string().c_str(), &image_width, &image_height, NULL, 4);
        if (image_data == NULL)
            return false;

        // Create a OpenGL texture identifier
        GLuint image_texture;
        glGenTextures(1, &image_texture);
        glBindTexture(GL_TEXTURE_2D, image_texture);

        // Setup filtering parameters for display
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        stbi_image_free(image_data);

        out_texture = image_texture;
        out_width   = image_width;
        out_height  = image_height;

        return true;
    }
}

namespace
{
    ALenum get_openal_format(const SDL_AudioSpec* spec)
    {
        if ((spec->channels == 1) && (spec->format == AUDIO_U8))
        {
            return AL_FORMAT_MONO8;
        }
        else if ((spec->channels == 1) && (spec->format == AUDIO_S16SYS))
        {
            return AL_FORMAT_MONO16;
        }
        else if ((spec->channels == 2) && (spec->format == AUDIO_U8))
        {
            return AL_FORMAT_STEREO8;
        }
        else if ((spec->channels == 2) && (spec->format == AUDIO_S16SYS))
        {
            return AL_FORMAT_STEREO16;
        }
        else if ((spec->channels == 1) && (spec->format == AUDIO_F32SYS))
        {
            return alIsExtensionPresent("AL_EXT_FLOAT32") ? alGetEnumValue("AL_FORMAT_MONO_FLOAT32") : AL_NONE;
        }
        else if ((spec->channels == 2) && (spec->format == AUDIO_F32SYS))
        {
            return alIsExtensionPresent("AL_EXT_FLOAT32") ? alGetEnumValue("AL_FORMAT_STEREO_FLOAT32") : AL_NONE;
        }
        return AL_NONE;
    }
}

void Demo::Setup()
{
    auto&      window      = Engine::GetWindow();
    const auto window_size = window.GetSize();
    viewport.x             = 0;
    viewport.y             = 0;
    viewport.width         = window_size.x;
    viewport.height        = window_size.y;
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    window.Clear(background_color);

    example_image.loaded = LoadTextureFromFile(assets::locate_asset("Assets/images/duck.png"), example_image.handle, example_image.width, example_image.height);

    // Initialize OpenAL
    al_device  = alcOpenDevice(nullptr);
    al_context = alcCreateContext(al_device, nullptr);
    alcMakeContextCurrent(al_context);

    alGenBuffers(2, alBufferHandles);
    alGenSources(2, alSourceHandles);

    ALenum  format = 0;
    ALvoid* data   = nullptr;
    ALsizei size   = 0;
    ALsizei freq   = 0;

    SDL_AudioSpec wavSpec;
    Uint32        wavLength;
    Uint8*        wavBuffer;
    const auto    e_path = assets::locate_asset("Assets/audio/duck-quacking-loudly-three-times.wav");
    if (SDL_LoadWAV(e_path.string().c_str(), &wavSpec, &wavBuffer, &wavLength) == nullptr)
    {
        throw_error_message("Failed to load WAV file: ", e_path, SDL_GetError());
    }
    format = get_openal_format(&wavSpec);
    data   = wavBuffer;
    size   = gsl::narrow_cast<ALsizei>(wavLength);
    freq   = wavSpec.freq;
    alBufferData(alBufferHandles[0], format, data, size, freq);
    SDL_FreeWAV(wavBuffer);
    alSourcei(alSourceHandles[0], AL_BUFFER, gsl::narrow_cast<ALint>(alBufferHandles[0]));

    int        channels, sample_rate;
    short*     output;
    const auto stereo_path = assets::locate_asset("Assets/audio/duck_vocalizations.ogg");
    int        samples     = stb_vorbis_decode_filename(stereo_path.string().c_str(), &channels, &sample_rate, &output);
    if (samples == -1)
    {
        throw_error_message("Failed to load OGG file: ", stereo_path);
    }

    format = (channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    data   = output;
    size   = samples * channels * int(sizeof(short));
    freq   = sample_rate;
    alBufferData(alBufferHandles[1], format, data, size, freq);
    free(output);
    alSourcei(alSourceHandles[1], AL_BUFFER, gsl::narrow_cast<ALint>(alBufferHandles[1]));

    ImGuiHelper::Initialize(window.GetSDLWindow(), window.GetGLContext());

    window.SetEventCallback(ImGuiHelper::FeedEvent);
}

void Demo::Shutdown()
{
    if (example_image.loaded)
    {
        glDeleteTextures(1, &example_image.handle);
    }

    alDeleteSources(2, alSourceHandles);
    alDeleteBuffers(2, alBufferHandles);

    alcMakeContextCurrent(nullptr);
    alcDestroyContext(al_context);
    alcCloseDevice(al_device);

    ImGuiHelper::Shutdown();
}

void Demo::Draw() const
{
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    Engine::GetWindow().Clear(background_color);
}

void Demo::ImGuiDraw()
{
    viewport = ImGuiHelper::Begin();

    ImGui::Begin("OpenGL Texture Test");
    if (example_image.loaded)
    {
        ImGui::Text("handle = %d", example_image.handle);
        ImGui::Text("size = %d x %d", example_image.width, example_image.height);
        ImGui::Image(static_cast<ImTextureID>(example_image.handle), ImVec2(static_cast<float>(example_image.width), static_cast<float>(example_image.height)));
    }
    else
    {
        ImGui::Text("%s", "Failed to load texture image...");
    }
    ImGui::End();

    ImGui::Begin("Audio Test");
    {
        if (ImGui::Button("Play Mono SFX"))
        {
            alSourcePlay(alSourceHandles[0]);
        }
        ImGui::SameLine();
        if (ImGui::Button("Play Stereo SFX"))
        {
            alSourcePlay(alSourceHandles[1]);
        }
    }
    ImGui::End();

    // Keyboard Information Window
    ImGui::Begin("Keyboard Information");
    {
        const auto&  input       = Engine::GetInput();
        const double currentTime = ImGui::GetTime();

        // Update highlights and add new events
        for (auto key = CS230::Input::Keys::A; key < CS230::Input::Keys::Count; ++key)
        {
            if (input.KeyJustPressed(key))
            {
                keyEventHistory.push_front({ key, "PRESSED", currentTime });
                keyHighlightTimer[key] = HIGHLIGHT_DURATION;
            }
            if (input.KeyJustReleased(key))
            {
                keyEventHistory.push_front({ key, "RELEASED", currentTime });
                keyHighlightTimer[key] = HIGHLIGHT_DURATION;
            }
        }

        // Limit history size
        while (keyEventHistory.size() > 20)
        {
            keyEventHistory.pop_back();
        }

        // Update highlight timers
        for (auto& [key, timer] : keyHighlightTimer)
        {
            timer -= static_cast<double>(ImGui::GetIO().DeltaTime);
        }

        // Display current pressed keys with highlights
        ImGui::Text("Currently Pressed Keys:");
        bool anyPressed = false;
        for (auto key = CS230::Input::Keys::A; key < CS230::Input::Keys::Count; ++key)
        {
            if (input.KeyDown(key))
            {
                anyPressed = true;
                // Highlight recently pressed/released keys
                if (keyHighlightTimer[key] > 0)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // Yellow highlight
                    ImGui::Text(">>> %s <<<", CS230::to_string(key));
                    ImGui::PopStyleColor();
                }
                else
                {
                    ImGui::Text("  %s", CS230::to_string(key));
                }
            }
        }
        if (!anyPressed)
        {
            ImGui::TextDisabled("(no keys pressed)");
        }

        ImGui::Separator();
        ImGui::Text("Recent Key Events:");

        // Display recent events
        if (keyEventHistory.empty())
        {
            ImGui::TextDisabled("(no recent events)");
        }
        else
        {
            for (const auto& event : keyEventHistory)
            {
                const double age = currentTime - event.timeStamp;
                if (age < 5.0) // Show events from last 5 seconds
                {
                    const float alpha = static_cast<float>(1.0 - (age / 5.0));                          // Fade out over time
                    ImVec4      color = (event.action == "PRESSED") ? ImVec4(0.0f, 1.0f, 0.0f, alpha) : // Green for pressed
                                       ImVec4(1.0f, 0.0f, 0.0f, alpha);                            // Red for released

                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                    ImGui::Text("[%.2fs ago] %s %s", age, CS230::to_string(event.key), event.action.c_str());
                    ImGui::PopStyleColor();
                }
            }
        }
    }
    ImGui::End();

    ImGuiHelper::End();
}
