//
// Created by root on 19/1/20.
//

#pragma once
#ifndef ENGINE_BILLBOARDSYSTEM_H
#define ENGINE_BILLBOARDSYSTEM_H

#include <vector>
#include <Billboard.h>
#include "Component.h"
#include "tags.h"
#include "Rendering.h"
#include "Engine.h"

namespace Engine {
    struct BillboardSystem : public SystemInterface {
        std::shared_ptr<Mesh> quad_mesh;
        void update(Engine::deltaTime time) override;

        void initialize() override;
    };

}

#endif //ENGINE_BILLBOARDSYSTEM_H
