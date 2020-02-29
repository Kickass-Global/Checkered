//
// Created by root on 29/2/20.
//

#ifndef ENGINE_SOUNDSYSTEM_H
#define ENGINE_SOUNDSYSTEM_H

#include <al.h>
#include <alc.h>
#include <Engine.h>


namespace Engine {
    class SoundSystem : public Engine::SystemInterface {
        ALCdevice *device;
    public:
        void initialize() override;

        void update(Engine::deltaTime /*elapsed*/) override;
    };
}

#endif //ENGINE_SOUNDSYSTEM_H
