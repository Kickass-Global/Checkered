//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#ifndef ENGINE_SCENERYSYSTEM_H
#define ENGINE_SCENERYSYSTEM_H

#include <Component.h>
#include <PhysicsActor.h>

namespace Engine {
    class ScenerySystem : public Engine::SystemInterface {
    public:
        Component::EventDelegate<Component::PhysicsActor *> onActorCreated;

        ScenerySystem() : onActorCreated("onActorCreated") {}

        void initialize() override;

        void update(deltaTime /*elapsedTime*/) override;
    };
}


#endif //ENGINE_SCENERYSYSTEM_H
