//
// Created by root on 29/2/20.
//// Refernces :: Used code from https://indiegamedev.net/2020/02/15/the-complete-guide-to-openal-with-c-part-1-playing-a-sound/

#pragma once

#ifndef ENGINE_SOUNDSYSTEM_H
#define ENGINE_SOUNDSYSTEM_H

#include <al.h>
#include <alc.h>
#include "Engine.h"

#include"Vehicle.h"

#include <PxPhysicsAPI.h>
#include <vehicle/PxVehicleDrive4W.h>


#include "systeminterface.hpp"
#include "EventHandler.h"

#include <GLFW/glfw3.h>

#include "Events/Events.h"

#include <fstream>
#include <string>
#include <vector>

#define alCall(function, ...) alCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define alcCall(function, device, ...) alcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)

namespace Engine {
    class SoundSystem : public Engine::SystemInterface {
        
        ALCdevice* device;
        ALCcontext* openALContext;
       
        ALuint sourceHorn;
        ALuint sourceAcceleration;
        ALuint sourceBreaking;
        ALuint sourceMusic;
        ALuint sourceDriving;
        ALuint sourceCollision;
        ALuint sourcePassengerDlivered;

        float volume = 0.5f;

        void onKeyDown(const Component::EventArgs<int>& args);
        void onKeyUp(const Component::EventArgs<int>& args);
        void onKeyPress(const Component::EventArgs<int>& args);

        
    public:
       
        std::shared_ptr<Component::Vehicle> playerVehicle;

        std::shared_ptr<EventHandler<int>> onKeyPressHandler;
        std::shared_ptr<EventHandler<int>> onKeyDownHandler;
        std::shared_ptr<EventHandler<int>> onKeyUpHandler;

        void volumeDown();
        void volumeUp();
      
        
        void initialize() override;
        void update(Engine::deltaTime /*elapsed*/) override;
        
        bool Engine::SoundSystem::check_al_errors(const std::string& filename, const std::uint_fast32_t line)
        {
            ALenum error = alGetError();
            if (error != AL_NO_ERROR)
            {
                std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
                switch (error)
                {
                case AL_INVALID_NAME:
                    std::cerr << "AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function";
                    break;
                case AL_INVALID_ENUM:
                    std::cerr << "AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function";
                    break;
                case AL_INVALID_VALUE:
                    std::cerr << "AL_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                    break;
                case AL_INVALID_OPERATION:
                    std::cerr << "AL_INVALID_OPERATION: the requested operation is not valid";
                    break;
                case AL_OUT_OF_MEMORY:
                    std::cerr << "AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory";
                    break;
                default:
                    std::cerr << "UNKNOWN AL ERROR: " << error;
                }
                std::cerr << std::endl;
                return false;
            }
            return true;
        }

        template<typename alFunction, typename... Params>
        auto alCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alFunction function,
            Params... params)
            ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, decltype(function(params...))>
        {
            auto ret = function(std::forward<Params>(params)...);
            check_al_errors(filename, line);
            return ret;
        }

        template<typename alFunction, typename... Params>
        auto alCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alFunction function,
            Params... params)
            ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
        {
            function(std::forward<Params>(params)...);
            return check_al_errors(filename, line);
        }



        bool Engine::SoundSystem::check_alc_errors(const std::string& filename, const std::uint_fast32_t line, ALCdevice* device)
        {
            ALCenum error = alcGetError(device);
            if (error != ALC_NO_ERROR)
            {
                std::cerr << "***ERROR*** (" << filename << ": " << line << ")\n";
                switch (error)
                {
                case ALC_INVALID_VALUE:
                    std::cerr << "ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function";
                    break;
                case ALC_INVALID_DEVICE:
                    std::cerr << "ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function";
                    break;
                case ALC_INVALID_CONTEXT:
                    std::cerr << "ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function";
                    break;
                case ALC_INVALID_ENUM:
                    std::cerr << "ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function";
                    break;
                case ALC_OUT_OF_MEMORY:
                    std::cerr << "ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function";
                    break;
                default:
                    std::cerr << "UNKNOWN ALC ERROR: " << error;
                }
                std::cerr << std::endl;
                return false;
            }
            return true;
        }

        template<typename alcFunction, typename... Params>
        auto alcCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alcFunction function,
            ALCdevice* device,
            Params... params)
            ->typename std::enable_if_t<std::is_same_v<void, decltype(function(params...))>, bool>
        {
            function(std::forward<Params>(params)...);
            return check_alc_errors(filename, line, device);
        }

        template<typename alcFunction, typename ReturnType, typename... Params>
        auto alcCallImpl(const char* filename,
            const std::uint_fast32_t line,
            alcFunction function,
            ReturnType& returnValue,
            ALCdevice* device,
            Params... params)
            ->typename std::enable_if_t<!std::is_same_v<void, decltype(function(params...))>, bool>
        {
            returnValue = function(std::forward<Params>(params)...);
            return check_alc_errors(filename, line, device);
        }




        std::int32_t convert_to_int(char* buffer, std::size_t len);
        bool load_wav_file_header(std::ifstream& file,
            std::uint8_t& channels,
            std::int32_t& sampleRate,
            std::uint8_t& bitsPerSample,
            ALsizei& size);
       
        char* load_wav(const std::string& filename,
            std::uint8_t& channels,
            std::int32_t& sampleRate,
            std::uint8_t& bitsPerSample,
            ALsizei& size);
        
        ALuint load_sound(std::string s);
       
        ALuint load_looping_sound(std::string s);
        
        int playSound(ALuint s);
    
        int stopSound(ALuint s);

 };
}

#endif //ENGINE_SOUNDSYSTEM_H
