#pragma once
#ifndef ENGINE_COMPONENTS_SCENERY_HPP
#define ENGINE_COMPONENTS_SCENERY_HPP

#include <Engine.h>
#include <material.hpp>
#include <Mesh.h>
#include <SceneComponent.h>
#include <PhysicsActor.h>

namespace Component {

    /**
     * Scenery components are things that can be placed into the world and have collision properties
     */
    class Scenery : public SceneComponent {
    public:
        ComponentReference<ClassId::PhysicsActor> actor;
        std::vector<ComponentReference<ClassId::Transform>> instances;

        Scenery(std::shared_ptr<Mesh> &mesh, std::shared_ptr<Material> &material) :
            actor(Engine::createComponent<PhysicsActor>(mesh)->id()) {}
    };

}

#endif