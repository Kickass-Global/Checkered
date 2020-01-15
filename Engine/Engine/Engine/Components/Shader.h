//
// Created by root on 11/1/20.
//

#ifndef ENGINE_SHADER_H
#define ENGINE_SHADER_H

#include <string>
#include "ComponentId.h"

namespace Component {

    class Shader {
        ComponentId m_id;

    public:
        ComponentId id() const { return m_id; }
    };

}

#endif //ENGINE_SHADER_H
