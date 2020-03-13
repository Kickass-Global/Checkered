//
// Created by root on 11/1/20.
//

#pragma once

#ifndef ENGINE_CAMERA_H
#define ENGINE_CAMERA_H

#include <vector>
#include "ComponentId.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "ComponentBase.h"

namespace Component {

	class Camera : public ComponentBase {

	public:

		bool is_dirty = false;

		struct Viewport {
			float x, y, width, height;
		} viewport;

		glm::vec3 position;
		glm::quat rotation;
		glm::quat local_rotation{ 1,0,0,0 };
		glm::mat4 view{ 1 };

		std::shared_ptr<ComponentInterface> target;
		glm::vec3 offset = { 0,4,-10 };

		Camera();
	};

}

#endif //ENGINE_CAMERA_H
