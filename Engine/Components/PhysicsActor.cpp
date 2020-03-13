//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#include "PhysicsActor.h"

glm::mat4 Component::PhysicsActor::getWorldTransform() {

    if (!actor) return glm::mat4{1};
    auto T = actor->getGlobalPose();
    return glm::translate(glm::vec3{T.p.x, T.p.y, T.p.z}) * glm::mat4_cast(glm::quat{T.q.w, T.q.x, T.q.y, T.q.z});
}

PhysicsActor::PhysicsActor(std::shared_ptr<Component::Mesh> &mesh) : mesh(mesh), actor(nullptr), worldTransform() {}
