// Engine.cpp : Defines the entry point for the application.
//

#include "main.h"
#include "Systems/Pipeline/EntityLoader.h"
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
		PxTolerancesScale(), recordMemoryAllocations, mPvd);
	if (!mPhysics)
		Engine::assertLog(false, "PxCreatePhysics failed!");

    using namespace Engine;

    auto running = true;

    Rendering::RenderingSystem renderingSystem;
    renderingSystem.initialize();

    Debug::LiveReloadSystem liveReloadSystem;

    // hookup inputs from current window
    Input::InputSystem::initialize(renderingSystem.getWindow());

    // hookup key press event with camera system
    Camera::CameraSystem cameraSystem;

    Input::InputSystem::onKeyPress += cameraSystem.onKeyPressHandler;
    Input::InputSystem::onKeyDown += cameraSystem.onKeyDownHandler;
    Input::InputSystem::onKeyUp += cameraSystem.onKeyUpHandler;

    Rendering::RenderingSystem::onWindowSizeChanged += cameraSystem.onWindowSizeHandler;

    // simulate loading a complex game object

    auto box_object = Pipeline::EntityLoader::load<Component::GameObject>(
            "Assets/Objects/box.json");
    box_object->worldTransform = glm::translate(glm::vec3{0, 0, -5});
    box_object->attachComponent(Component::Dirty::id());
    box_object->attachComponent(Component::Visible::id());

    { // hack; todo: not this
        auto components = Component::Index::componentsOf(box_object->id());

        auto it = std::find_if(components.begin(), components.end(),
                               [](auto component) {
                                   return component.classId() ==
                                          Component::ClassId::Mesh;
                               });

        auto components2 = Component::Index::componentsOf(*it);
        auto it2 = std::find_if(components2.begin(), components2.end(),
                                [](auto component) {
                                    auto cid = component.classId();
                                    return cid ==
                                           Component::ClassId::Program;
                                });

        it->data<Component::Mesh>()->shader = *it2;

    } // hack

    // make a default camera
    auto camera = Engine::createComponent<Component::Camera>();

    // setup a game clock

    auto start = std::chrono::high_resolution_clock::now();
    auto end = start;

    while (running) {

        // frametime is a measure of how 'on-time' a frame is... <1 early, >1 late.
        frametime elapsed =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                        end - start).count();

        //Engine::log<module>("Frametime: ", elapsed);

        Engine::EventSystem::update(elapsed);
        liveReloadSystem.update(elapsed);
        cameraSystem.update(elapsed);
        renderingSystem.update(elapsed);

        start = end;
        end = std::chrono::high_resolution_clock::now();
    }
}
