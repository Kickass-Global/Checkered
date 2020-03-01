//
// Created by root on 15/1/20.
//

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
    class SceneComponent : public ComponentBase<ClassId::SceneComponent> {
    public:
		SceneComponent *m_parent = nullptr;
        std::vector<SceneComponent *> m_children;
        glm::mat4 m_localTransform;

        /**
         * Returns the world transformation matrix for this scene component.
         * This is done by traversing the SceneComponent hierarchy and multiplying each component's local
         * transformation matrix together in post-order (e.g. from top to bottom).
         */
        glm::mat4 getWorldTransform() {

            auto parent = this->m_parent;
            auto transform = glm::mat4(1);

            // use a stack to reverse the order that we apply transformations in.
            std::stack<SceneComponent *> stack;
            while (parent) {
                stack.push(parent);
                parent = parent->m_parent;
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
