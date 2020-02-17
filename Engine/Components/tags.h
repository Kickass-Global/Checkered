//
// Created by root on 11/1/20.
//

#ifndef ENGINE_TAGS_H
#define ENGINE_TAGS_H

#include <glm/fwd.hpp>
#include <glm/detail/type_mat4x4.hpp>
#include "ComponentId.h"
#include "ComponentBase.h"

namespace Component {

    namespace {
        const char dirty[] = "Dirty";
        const char visible[] = "Visible";
    }

    class Dirty : public ComponentTag<dirty> {
    };

    class Visible : public ComponentTag<visible> {
    };

    static Dirty dirty();
    static Visible visible();
}

#endif //ENGINE_TAGS_H
