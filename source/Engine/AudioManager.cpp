// #include "AudioManager.h"
// #include "Engine/Engine.hpp"
// #include "Engine/Logger.hpp"
// #include "Engine/Path.hpp"
// #include "Engine/Error.hpp"
// #include <SDL.h>

// namespace CS230
// {
//     AudioManager::AudioManager() {}

//     AudioManager::~AudioManager()
//     {

//         music_cache.clear();
//         sound_cache.clear();
//         Mix_CloseAudio();
//         Mix_Quit();
//     }

//     void AudioManager::Init()
//     {

//         if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
//         {
//             throw_error_message("Failed to initialize SDL_AUDIO", SDL_GetError());
//         }

//         int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
//         if ((Mix_Init(flags) & flags) != flags)
//         {
//             throw_error_message("Failed to initialize SDL_mixer (MP3, OGG)", Mix_GetError());
//         }

//         if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
//         {
//             throw_error_message("Failed to open audio device", Mix_GetError());
//         }

//         Engine::GetLogger().LogEvent("Audio Manager Initialized");
//     }

//     void AudioManager::Shutdown()
//     {
//         Unload();
//         Mix_CloseAudio();
//         Mix_Quit();
//         SDL_QuitSubSystem(SDL_INIT_AUDIO);
//         Engine::GetLogger().LogEvent("Audio Manager Shutdown");
//     }

//     Mix_Chunk* AudioManager::LoadSound(const std::filesystem::path& file_name)
//     {
//         const std::string path_string = file_name.string();

//         if (sound_cache.count(path_string))
//         {
//             return sound_cache.at(path_string).get();
//         }

//         std::filesystem::path asset_path;
//         try {
//             asset_path = assets::locate_asset(file_name);
//         } catch(const std::exception& e) {
//             Engine::GetLogger().LogError(e.what());
//             return nullptr;
//         }

//         Mix_Chunk* chunk = Mix_LoadWAV(asset_path.string().c_str());
//         if (chunk == nullptr)
//         {
//             Engine::GetLogger().LogError("Failed to load sound: " + path_string + " | " + Mix_GetError());
//             return nullptr;
//         }

//         sound_cache[path_string] = std::unique_ptr<Mix_Chunk, SoundDeleter>(chunk);
//         Engine::GetLogger().LogDebug("Loaded Sound: " + path_string);
//         return chunk;
//     }

//     Mix_Music* AudioManager::LoadMusic(const std::filesystem::path& file_name)
//     {
//         const std::string path_string = file_name.string();
//         if (music_cache.count(path_string))
//         {
//             return music_cache.at(path_string).get();
//         }
        
//         std::filesystem::path asset_path;
//          try {
//             asset_path = assets::locate_asset(file_name);
//         } catch(const std::exception& e) {
//             Engine::GetLogger().LogError(e.what());
//             return nullptr;
//         }

//         Mix_Music* music = Mix_LoadMUS(asset_path.string().c_str());
//         if (music == nullptr)
//         {
//             Engine::GetLogger().LogError("Failed to load music: " + path_string + " | " + Mix_GetError());
//             return nullptr;
//         }

//         music_cache[path_string] = std::unique_ptr<Mix_Music, MusicDeleter>(music);
//         Engine::GetLogger().LogDebug("Loaded Music: " + path_string);
//         return music;
//     }

//     int AudioManager::PlaySound(Mix_Chunk* sound, int loops)
//     {
//         if (sound == nullptr)
//         {
//             Engine::GetLogger().LogError("Attempted to play null sound chunk");
//             return -1;
//         }

//         return Mix_PlayChannel(-1, sound, loops);
//     }

//     int AudioManager::PlayMusic(Mix_Music* music, int loops)
//     {
//         if (music == nullptr)
//         {
//             Engine::GetLogger().LogError("Attempted to play null music");
//             return -1;
//         }
//         return Mix_PlayMusic(music, loops);
//     }

//     void AudioManager::Unload()
//     {

//         music_cache.clear();
//         sound_cache.clear();
//         Engine::GetLogger().LogEvent("Cleared all audio caches");
//     }
// }