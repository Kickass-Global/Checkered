//
// Created by root on 19/1/20.
//

#ifndef ENGINE_BILLBOARD_H
#define ENGINE_BILLBOARD_H

#include "Component.h"

namespace Component {

    class Billboard : public ComponentBase<ClassId::Billboard> {
    public:
        ComponentId mesh;
        ComponentId material;
        struct Rectangle {
            int x;
            int y;
            int width;
            int height;
        } plot;
    };

}


#endif //ENGINE_BILLBOARD_H
