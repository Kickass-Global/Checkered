//
// Created by root on 11/1/20.
//

#ifndef ENGINE_CAMERASYSTEM_H
#define ENGINE_CAMERASYSTEM_H

#include <map>
#include <algorithm>
#include <string>
#include <chrono>
#include <functional>
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#define _USE_MATH_DEFINES

#include <math.h>
#include <Vehicle.h>

#include "ComponentId.h"
#include "Camera.h"
#include "systeminterface.hpp"
#include "EventHandler.h"
#include "Engine.h"
#include "Events/Events.h"
#include "SceneComponent.h"

namespace Camera {

	namespace {
		char module[] = "Camera";
	}

	class CameraSystem : public Engine::SystemInterface {

	public:

		static Component::ComponentId id();

		std::shared_ptr<EventHandler<int>> onKeyPressHandler;
		std::shared_ptr<EventHandler<int>> onKeyDownHandler;
		std::shared_ptr<EventHandler<int>> onKeyUpHandler;
		std::shared_ptr<EventHandler<int, int>> onWindowSizeHandler;

		CameraSystem() {

			// create event handlers

			onKeyPressHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onKeyPress);
			onKeyDownHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onKeyDown);
			onKeyUpHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onKeyUp);
			onWindowSizeHandler = Engine::EventSystem::createHandler(this, &CameraSystem::onWindowSizeChanged);

		}

		float x = 0;
		float y = 0;
		float u = 0;

		float sensitivity = 100.0f;
		float increment = static_cast<float>(M_PI) / 180.0f;

		std::set<int> keys;

		void update(Engine::deltaTime elapsed) override;

		void onWindowSizeChanged(const Component::EventArgs<int, int> &args);

		void onKeyDown(const Component::EventArgs<int> &args);

		void onKeyUp(const Component::EventArgs<int> &args);

		void onKeyPress(const Component::EventArgs<int> &args);

	};

}

#endif //ENGINE_CAMERASYSTEM_H
