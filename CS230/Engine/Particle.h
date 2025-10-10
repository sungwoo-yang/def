#pragma once
#include "GameObjectManager.h"
#include "Component.h"
#include "../GameObjectTypes.h"
#include "../Engine/Engine.h"

namespace CS230 {
    class Particle : public GameObject {
    public:
        Particle(const std::filesystem::path& sprite_file);
        void Start(Math::vec2 pos, Math::vec2 vel , double max_life);
        void Update(double dt) override;
        void Draw(Math::TransformationMatrix camera_matrix) override;
        bool Alive() {
            return  life > 0;
        }
        GameObjectTypes Type() { return GameObjectTypes::Particle; }
    private:
        double life;
    };

    template<typename T>
    class ParticleManager : public Component {
    public:
        ParticleManager();
        ~ParticleManager();
        void Emit(int count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread);
    private:
        std::vector<T*> particles;
        int index;
    };

    template<typename T>
    ParticleManager<T>::ParticleManager() : index(0) {
        for (int i = 0; i < T::MaxCount; ++i) {
            T* p = new T();
            particles.push_back(p);
            Engine::GetGameStateManager().GetGSComponent<GameObjectManager>()->Add(p);
        }
    }

    template<typename T>
    ParticleManager<T>::~ParticleManager() {
        particles.clear();
    }

    template<typename T>
    void ParticleManager<T>::Emit(int count, Math::vec2 emitter_position, Math::vec2 emitter_velocity, Math::vec2 direction, double spread) {
        for (int i = 0; i < count; ++i) {
            if (particles[index]->Alive()) {
                Engine::GetLogger().LogDebug("Particle overwritten");
            }

            double angle_variation = 0.0;
            if (spread != 0.0) {
                angle_variation = ((rand() % static_cast<int>(spread * 1024)) / 1024.0) - (spread / 2.0);
            }

            Math::vec2 random_magnitude = direction * (((rand() % 1024) / 2048.0) + 0.5);
            Math::vec2 rotated = {
                random_magnitude.x * std::cos(angle_variation) - random_magnitude.y * std::sin(angle_variation),
                random_magnitude.x * std::sin(angle_variation) + random_magnitude.y * std::cos(angle_variation)
            };

            Math::vec2 particle_velocity = rotated + emitter_velocity;

            particles[index]->Start(emitter_position, particle_velocity, T::MaxLife);

            ++index;
            if (index >= static_cast<int>(particles.size())) {
                index = 0;
            }
        }
    }
}
