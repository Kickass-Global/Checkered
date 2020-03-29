//
// Created by Jackson Cougar Wiebe on 2/13/2020.
//

#pragma once

#ifndef ENGINE_VEHICLESYSTEM_HPP
#define ENGINE_VEHICLESYSTEM_HPP

#include <SystemInterface.hpp>
#include <ComponentId.h>
#include <Vehicle.h>

namespace Engine {
    class vehicleSystem : public Engine::SystemInterface {
    public:
        Component::EventDelegate<Vehicle*> onVehicleCreated;

        vehicleSystem() : onVehicleCreated("onVehicleCreated") {}

        void initialize() override;
        void update(deltaTime /*deltaTime*/) override;
    };

}

#endif //ENGINE_VEHICLESYSTEM_HPP
