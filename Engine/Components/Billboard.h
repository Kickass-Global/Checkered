//
// Created by root on 19/1/20.
//

#pragma once
#ifndef ENGINE_BILLBOARD_H
#define ENGINE_BILLBOARD_H

#include "Component.h"

namespace Component {


    class Billboard : public ComponentBase {
    public:
		std::shared_ptr<class MeshInstance> mesh_instance;
        struct Rectangle {
            float x;
            float y;
            float width;
            float height;
        } plot;
        glm::vec2 anchor = {1, 1};
    };

}


#endif //ENGINE_BILLBOARD_H
