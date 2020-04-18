//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#pragma once

#ifndef ENGINE_SYSTEMINTERFACE_HPP
#define ENGINE_SYSTEMINTERFACE_HPP

#include <glad/glad.h>
//
#include <GLFW/glfw3.h>
#include <foundation/PxVec3.h>
#include <PxPhysicsAPI.h>
#include <map>
#include <typeindex>
#include <vector>
#include <memory>
#include <chrono>
#include <execution>
namespace Engine {

    typedef float deltaTime;
    typedef std::chrono::duration<float> floatMilliseconds;

    class SystemInterface {

    protected:
        class EngineSystem *enginePtr = nullptr;

        std::map<std::type_index, std::unique_ptr<Engine::SystemInterface>> subsystems;
    public:
        int order = 1;

        class EngineSystem *getEngine() { return enginePtr; }

        virtual void update(Engine::deltaTime elapsed) {
            for (auto &subsystem : subsystems) {
                subsystem.second->early_update(elapsed);
            }

            for (auto &subsystem : subsystems) {
                subsystem.second->update(elapsed);
            }
            for (auto &subsystem : subsystems) {
                subsystem.second->late_update(elapsed);
            }
        }

        virtual void late_update(Engine::deltaTime /*elapsed*/) { /**/ }

        virtual void early_update(Engine::deltaTime /*elapsed*/) { /**/ }

        /**
         * Creates a new system to the engine; systems need to be default constructable.
         * @tparam T the type of system
         * @return returns a reference to the new system.
         */
        template<typename T>
        T *addSubSystem() {

            auto[it, emplaced] = subsystems.emplace(typeid(T), new T());

            if (emplaced) {
                it->second->enginePtr = enginePtr;
                it->second->initialize();
            }

            return static_cast<T *>(it->second.get());
        }

        template<typename T>
        T *getSubSystem() {

            static_assert(std::is_base_of<SystemInterface, T>::value);
            auto it = subsystems.find(typeid(T));
            if (it == subsystems.end()) return nullptr;
            else return dynamic_cast<T *>(it->second.get());
        }

        /**
         * Returns collection of sub-systems.
         */
        std::vector<Engine::SystemInterface *> getSubSystems() {

            std::vector<Engine::SystemInterface *> result;
            for (auto&[key, system] : subsystems) {
                result.push_back(system.get());
            }
            return result;
        }

        virtual void initialize() {}
    };

    void setupWheelsSimulationData(const float wheelMass,
                                   const float wheelMOI,
                                   const float wheelRadius,
                                   const float wheelWidth,
                                   const unsigned int numWheels,
                                   const physx::PxVec3 *wheelCenterActorOffsets,
                                   const physx::PxVec3 &chassisCMOffset,
                                   const float chassisMass,
                                   physx::PxVehicleWheelsSimData *wheelsSimData);
}

#endif //ENGINE_SYSTEMINTERFACE_HPP
