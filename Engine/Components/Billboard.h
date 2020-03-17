//
// Created by root on 19/1/20.
//

#pragma once
#ifndef ENGINE_BILLBOARD_H
#define ENGINE_BILLBOARD_H

#include "Component.h"
#include "glm/vec2.hpp"
#include "texture.hpp"
#include <Types.hpp>

namespace Component {


    class Billboard : public ComponentBase {
    public:
        std::shared_ptr<class PaintedMesh> mesh_instance;
        Engine::Rectangle plot;
        glm::vec2 anchor = {1, 1};

        Billboard() = default;
        explicit Billboard(std::shared_ptr<Texture> sprite);
    };

}


#endif //ENGINE_BILLBOARD_H
