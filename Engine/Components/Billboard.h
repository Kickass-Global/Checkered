//
// Created by root on 19/1/20.
//

#pragma once
#ifndef ENGINE_BILLBOARD_H
#define ENGINE_BILLBOARD_H

#include "Component.h"
#include "glm/vec2.hpp"
#include "texture.hpp"
#include "material.hpp"
#include <Types.hpp>
#include <Pipeline/Library.h>

namespace Component {


    class Billboard : public ComponentBase {
    public:
        std::shared_ptr<class PaintedMesh> mesh_instance;
        Engine::BoxModel plot;
        Engine::RelativeAnchor src = { 1, 1 };
        Engine::RelativeAnchor dst = { 1, 1 };

        Billboard() = default;
        explicit Billboard(const std::shared_ptr<Texture>& billboard_image);
    };

}


#endif //ENGINE_BILLBOARD_H
