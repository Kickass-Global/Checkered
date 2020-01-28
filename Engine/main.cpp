// Engine.cpp : Defines the entry point for the application.
//

#include <functional>

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
#include "Systems/Damage/damagesystem.hpp"
#include <PxPhysicsAPI.h>


int main() {

    using namespace physx;

	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;

	auto mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
		gDefaultErrorCallback);
	if (!mFoundation)
        Engine::assertLog(false, "PxCreateFoundation failed!");

	bool recordMemoryAllocations = true;

	auto mPvd = PxCreatePvd(*mFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);


    auto mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
                                    PxTolerancesScale(),
                                    recordMemoryAllocations, mPvd);
    if (!mPhysics)
        Engine::assertLog(false, "PxCreatePhysics failed!");

    using namespace Engine;

    auto running = true;


    auto renderingSystem = std::make_shared<Rendering::RenderingSystem>();
    Engine::addSystem(renderingSystem);

    auto liveReloadSystem = std::make_shared<Debug::LiveReloadSystem>();
    Engine::addSystem(liveReloadSystem);

    auto inputSystem = std::make_shared<Input::InputSystem>();
    Engine::addSystem(inputSystem);

    // hookup inputs from current window
    inputSystem->initialize(renderingSystem->getWindow());

    // hookup key press event with camera system
    auto cameraSystem = std::make_shared<Camera::CameraSystem>();
    Engine::addSystem(cameraSystem);
    Engine::addSystem(std::make_shared<Engine::EventSystem>());
    Engine::addSystem(std::make_shared<Engine::DamageSystem>());

    Input::InputSystem::onKeyPress += cameraSystem->onKeyPressHandler;
    Input::InputSystem::onKeyDown += cameraSystem->onKeyDownHandler;
    Input::InputSystem::onKeyUp += cameraSystem->onKeyUpHandler;

    Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem->onWindowSizeHandler;

    // simulate loading a complex game object

    Component::Index;

    auto box_object = Pipeline::load<Component::GameObject>(
            "Assets/Objects/box.json");

    box_object->worldTransform = glm::translate(glm::vec3{0, 0, -5});
    box_object->attachComponent(Component::Dirty::id());
    box_object->attachComponent(Component::Visible::id());

//    { // hack; todo: not this
//        auto components = Component::Index::componentsOf(box_object->id());
//
//        auto it = std::find_if(components.begin(), components.end(),
//                               [](auto component) {
//                                   return component.classId() ==
//                                          Component::ClassId::Mesh;
//                               });
//
//        auto components2 = Component::Index::componentsOf(*it);
//        auto it2 = std::find_if(components2.begin(), components2.end(),
//                                [](auto component) {
//                                    auto cid = component.classId();
//                                    return cid ==
//                                           Component::ClassId::Program;
//                                });
//
//        it->data<Component::Mesh>()->shader = *it2;
//
//    } // hack

//    auto damage_object
//            = Engine::createComponent<Component::GameObject>("object");
//
//    auto damage_model
//            = Engine::createComponent<Component::Model>("model");
//
//    auto box_mesh = Pipeline::MeshLoader::load(
//            "Assets/Meshes/box.obj");
//
//    damage_model->parts.push_back(Component::Model::Part{});
//    damage_model->parts[0].variations.push_back(
//            Component::Model::Variation{0, box_mesh->id()}
//    );

//    std::function<void(const Component::EventArgs<int> &)> onKeyPress
//            = [damage_model](auto &args) {
//
//                auto key = std::get<0>(args.values);
//                Engine::log(key, " was pressed");
//                if (key == GLFW_KEY_D) {
//                    auto damage = Engine::createComponent<Component::Damage>();
//                    damage->damage_amount = 1;
//                    damage_model->attachComponent(damage->id());
//                }
//
//            };
//    auto debugHandler = Engine::EventSystem::createHandler(onKeyPress);
//    Input::InputSystem::onKeyPress += debugHandler;


    // make a default camera
    auto camera = Engine::createComponent<Component::Camera>();

    // setup a game clock

    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;

    while (running) {

        deltaTime elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start).count();


        for (const auto &system : Engine::systems()) {
            system->update(elapsed);
        }

        start = end;
        end = std::chrono::high_resolution_clock::now();
    }
}
