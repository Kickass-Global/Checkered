//
// Created by Jackson Cougar Wiebe on 3/1/2020.
//

#include "PhysicsActor.h"

PhysicsActor::PhysicsActor(std::shared_ptr<Component::Mesh> &mesh) : mesh(mesh), actor(nullptr) {}
