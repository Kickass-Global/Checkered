//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#pragma once

#ifndef ENGINE_SYSTEMINTERFACE_HPP
#define ENGINE_SYSTEMINTERFACE_HPP


#include <foundation/PxVec3.h>
#include <PxPhysicsAPI.h>

namespace Engine {

    typedef float deltaTime;

    class SystemInterface {
        std::vector<std::unique_ptr<Engine::SystemInterface>> subsystems;
    public:
        int order = 1;

        virtual void update(Engine::deltaTime /*elapsed*/) = 0;

        /**
         * Creates a new system to the engine; systems need to be default constructable.
         * @tparam T the type of system
         * @return returns a reference to the new system.
         */
        template<typename T>
        T *addSubSystem() {

            subsystems.push_back(std::make_unique<T>());
            subsystems.back()->initialize();
            return static_cast<T *>(subsystems.back().get());
        }

        /**
         * Returns collection of sub-systems.
         */
        const std::vector<std::unique_ptr<Engine::SystemInterface>> &getSubSystems() {

            return subsystems;
        }

        virtual void initialize() {
        }
    };

    void setupWheelsSimulationData(
        const float wheelMass, const float wheelMOI, const float wheelRadius, const float wheelWidth,
        const unsigned int numWheels, const physx::PxVec3 *wheelCenterActorOffsets,
        const physx::PxVec3 &chassisCMOffset, const float chassisMass, physx::PxVehicleWheelsSimData *wheelsSimData
    );
}

#endif //ENGINE_SYSTEMINTERFACE_HPP
