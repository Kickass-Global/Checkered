//
// Created by Jackson Cougar Wiebe on 2/8/2020.
//

#pragma once

#ifndef ENGINE_SCENECOMPONENTSYSTEM_HPP
#define ENGINE_SCENECOMPONENTSYSTEM_HPP

#include "../systeminterface.hpp"

namespace Component {
    class SceneComponentSystem : public Engine::SystemInterface {


    public:
		EventDelegate<PhysicsActor*> onActorCreated{ "onActorCreated" };

        void update(Engine::deltaTime /*elapsed*/) override;
        void initialize() override;

    };
}


#endif //ENGINE_SCENECOMPONENTSYSTEM_HPP
