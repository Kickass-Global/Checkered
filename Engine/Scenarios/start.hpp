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
            renderingSystem->onWindowSizeChanged += cameraSystem->onWindowSizeHandler;
            auto inputSystem = getEngine()->addSubSystem<Input::InputSystem>();
            renderingSystem->addSubSystem<Engine::BillboardSystem>();
            renderingSystem->addSubSystem<Engine::FontSystem>();

            getEngine()->createComponent<Component::Camera>();

            auto sprite = getEngine()->createMomentaryComponent<Component::Billboard>();
            sprite->plot = {0, 0, 640, 480};
            sprite->anchor = {0, 0};

            auto material = getEngine()->createMomentaryComponent<Component::Material>(
                getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Program>("Assets/Programs/billboard.json"));

            material->textures.push_back(
                getEngine()->createMomentaryComponent<Component::Texture>("Assets/Textures/Nature_Trees.png"));

            sprite->mesh_instance = getEngine()->createMomentaryComponent<PaintedMesh>(
                getEngine()->getSubSystem<Pipeline::Library>()->getAsset<Mesh>("Assets/Meshes/billboard_quad.obj"),
                material
            );

        }

        void update(deltaTime d) {

            auto fps = getEngine()->createMomentaryComponent<Component::Text>("Frametime: " + std::to_string(d));
            fps->font_size = 4;
            fps->anchor = { -1, 1 };
            fps->align = eAlign::left;

            auto prompt = getEngine()->createMomentaryComponent<Component::Text>("Press Start to Continue...");
            prompt->font_size = 12;
            prompt->anchor = {0, 0.2};
        }
    };
}

#endif //ENGINE_START_HPP
