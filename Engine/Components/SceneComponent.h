//
// Created by root on 15/1/20.
//

#pragma once

#ifndef ENGINE_SCENECOMPONENT_H
#define ENGINE_SCENECOMPONENT_H

#include <vector>
#include <stack>

#include "ComponentId.h"
#include "ComponentBase.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace Component {

    /**
     * A scene component has a transformation related with it; scene components can contain other scene components
     * to form a hierarchy of transformations which are relative to one another.
     */
    class SceneComponent : public ComponentBase {
    public:
		std::shared_ptr<SceneComponent> m_parent;;
        std::vector<std::shared_ptr<SceneComponent>> m_children;
		glm::mat4 m_localTransform = glm::mat4{ 1 };


		template <typename... Args>
		SceneComponent(std::shared_ptr<SceneComponent> parent, std::shared_ptr<Args>... children) :m_parent(parent)
		{
			addChildComponents(children...);
		}


        /**
         * Returns the world transformation matrix for this scene component.
         * This is done by traversing the SceneComponent hierarchy and multiplying each component's local
         * transformation matrix together in post-order (e.g. from top to bottom).
         */
        virtual glm::mat4 getWorldTransform() {

            auto parent = this->m_parent.get();
            auto transform = glm::mat4(1);

            // use a stack to reverse the order that we apply transformations in.
            std::stack<SceneComponent *> stack;
            while (parent) {
                stack.push(parent);
                parent = parent->m_parent.get();
            }

            while (!stack.empty()) {
                parent = stack.top();
                stack.pop();
                transform *= parent->m_localTransform;
            }
            transform *= m_localTransform;

            return transform;
        }
    };

}


#endif //ENGINE_SCENECOMPONENT_H
