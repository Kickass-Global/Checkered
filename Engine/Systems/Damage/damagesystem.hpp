//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#pragma once

#ifndef ENGINE_DAMAGESYSTEM_HPP
#define ENGINE_DAMAGESYSTEM_HPP


#include <numeric>
#include <algorithm>

#include "Engine.h"
#include "../../Components/damage.hpp"
#include "../../Components/EventHandler.h"
#include "../Events/Events.h"
#include "EventDelegate.h"
#include "SystemInterface.hpp"
#include "SystemInterface.hpp"
#include "tags.h"

namespace Engine {

    class DamageSystem : public Engine::SystemInterface {

        void initialize() override;

        void update(Engine::deltaTime elapsed) override;
    };
}

#endif //ENGINE_DAMAGESYSTEM_HPP
