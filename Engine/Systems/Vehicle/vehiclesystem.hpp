//
// Created by Jackson Cougar Wiebe on 2/13/2020.
//

#pragma once

#ifndef ENGINE_VEHICLESYSTEM_HPP
#define ENGINE_VEHICLESYSTEM_HPP

#include <systeminterface.hpp>
#include <ComponentId.h>

namespace Engine {
    class vehicleSystem : public Engine::SystemInterface {
    public:
        Component::EventDelegate<Component::ComponentId> onVehicleCreated;

        vehicleSystem() : onVehicleCreated("onVehicleCreated") {}

        void initialize() override;
        void update(deltaTime /*deltaTime*/) override;
    };

}

#endif //ENGINE_VEHICLESYSTEM_HPP
