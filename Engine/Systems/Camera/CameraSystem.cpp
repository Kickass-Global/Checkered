//
// Created by root on 11/1/20.
//

#include "CameraSystem.h"
#include "WorldTransform.h"

void ::Camera::CameraSystem::update(Engine::deltaTime elapsed) {

	for (auto& camera : getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Camera>()) {


		// check if the camera is attached to a component, get that components
		// transform and update the camera to look at that object.

		if (camera->target) {

			Engine::log<module, Engine::low>("Updating camera to look at #", camera->target->getId());

			const auto transforms = camera->target->getChildren().getComponentsOfType<WorldTransform>();
			if (!transforms.empty()) {
				auto transform = transforms[0];
				auto offset =
					glm::toMat4(camera->rotation) * glm::toMat4(camera->local_rotation) * glm::vec4(camera->offset, 1);

				// this line of code controls the camera returning to 'neutral' position behind the target...
				camera->rotation = glm::slerp(camera->rotation, transform->rotation, 0.3f); // todo, scale this with speed
				camera->position = transform->position + glm::vec3(offset);

				//Engine::log<module>(transform->rotation);

				glm::vec3 eye = camera->position;
				glm::vec3 target = transform->position;
				glm::vec3 world_up = { 0, 1, 0 };

				camera->view = glm::lookAt(eye, target, world_up);
			}
		}
		else {
			camera->view = glm::translate(camera->position);
		}

		camera->is_dirty = true;
	}

}

void ::Camera::CameraSystem::onWindowSizeChanged(const Component::EventArgs<int, int>& args) {

	log<module, high>(
		"onWindowSizeChanged=", std::get<0>(args.values), ", ", std::get<0>(args.values));

	auto&& width = std::get<0>(args.values);
	auto&& height = std::get<1>(args.values);

	for (auto&& camera : getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Camera>()) {
		camera->viewport.width = width;
		camera->viewport.height = height;
		camera->is_dirty = true;
	}
}

void ::Camera::CameraSystem::onKeyPress(const Component::EventArgs<int>& args) {
	//Engine::log<module, Engine::low>("onKeyPress=", std::get<0>(args.values));
}

void ::Camera::CameraSystem::onKeyUp(const Component::EventArgs<int>& args) {
	//Engine::log<module>("onKeyUp=", std::get<0>(args.values));
	keys.erase(std::get<0>(args.values));
}

void ::Camera::CameraSystem::onKeyDown(const Component::EventArgs<int>& args) {
	//Engine::log<module>("onKeyDown=", std::get<0>(args.values));
	keys.emplace(std::get<0>(args.values));
}

void ::Camera::CameraSystem::onGamepadStateChanged(
	const Component::EventArgs<GLFWgamepadstate, GLFWgamepadstate>& args
) {
	//Engine::log<module>("onGamepadStateChanged=", std::get<0>(args.values));

	//auto input = std::get<1>(args.values);
	//x += input.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
}

void ::Camera::CameraSystem::initialize() {

	SystemInterface::initialize();

	// create event handlers

	onKeyPressHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(this, &CameraSystem::onKeyPress);
	onKeyDownHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(this, &CameraSystem::onKeyDown);
	onKeyUpHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(this, &CameraSystem::onKeyUp);
	onWindowSizeHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(
		this, &CameraSystem::onWindowSizeChanged
	);
	onGamepadStateChangedHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(
		this, &CameraSystem::onGamepadStateChanged
	);

}
