//
// Created by Jackson Cougar Wiebe on 2/16/2020.
//

#pragma once

#ifndef ENGINE_WORLDTRANSFORM_H
#define ENGINE_WORLDTRANSFORM_H

#include <glm/fwd.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include "ComponentId.h"
#include "ComponentBase.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace Component {

	class WorldTransform : public ComponentBase {
	public:
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 position;

		glm::mat4 world_matrix{ 1 };

		WorldTransform() : world_matrix(1) {}

		explicit WorldTransform(glm::mat4 T) : world_matrix(T) {
			glm::vec4 perspective;
			glm::vec3 skew;
			glm::decompose(T, scale, rotation, position, skew, perspective);
		}
	};

}

#endif //ENGINE_WORLDTRANSFORM_H
