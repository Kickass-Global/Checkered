//
// Created by Jackson Cougar Wiebe on 3/13/2020.
//

#ifndef ENGINE_START_HPP
#define ENGINE_START_HPP

#include <Engine.h>
#include <material.hpp>
#include <Billboard.h>
#include <Mesh.h>
#include <Pipeline/Library.h>
#include <Camera/CameraSystem.h>
#include <Input/InputSystem.h>
#include <Rendering/fontsystem.hpp>
#include <Rendering/BillboardSystem.h>

#include "scenariointerface.hpp"

namespace Engine {
    class Start : public ScenarioInterface {
    public:
        explicit Start(EngineSystem *pSystem) : ScenarioInterface(pSystem) {}

        void load() override {

            auto cameraSystem = getEngine()->addSubSystem<::Camera::CameraSystem>();
            auto renderingSystem = getEngine()->addSubSystem<Rendering::RenderingSystem>();
            auto inputSystem = getEngine()->addSubSystem<Input::InputSystem>();
            renderingSystem->addSubSystem<Engine::BillboardSystem>();
            renderingSystem->addSubSystem<Engine::FontSystem>();

            auto sprite = getEngine()->createComponent<Component::Billboard>();
            sprite->plot = {10, 10, 100, 100};

            auto material = getEngine()->createComponent<Component::Material>(
                getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>("Assets/Programs/billboard.json"));

            material->textures.push_back(
                getEngine()->createComponent<Component::Texture>("Assets/Textures/Nature_Trees.png"));

            sprite->mesh_instance = getEngine()->createNamedComponent<PaintedMesh>(
                "billboard_mesh_instance",
                getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/billboard_quad.obj"),
                material
            );

            getEngine()->createComponent<Component::Camera>();
        }

        void update() {

            getEngine()->createMomentaryComponent<Component::Text>("FPS:", 1, glm::vec3{1, 0, 0});
        }
    };
}

#endif //ENGINE_START_HPP
