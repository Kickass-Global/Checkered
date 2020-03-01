//
// Created by root on 19/1/20.
//

#ifndef ENGINE_BILLBOARD_H
#define ENGINE_BILLBOARD_H

#include "Component.h"

namespace Component {


    class Billboard : public ComponentBase<ClassId::Billboard> {
    public:
        ComponentId mesh_instance;
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
