//
// Created by root on 11/1/20.
//

#ifndef ENGINE_DIRTY_H
#define ENGINE_DIRTY_H

#include "ComponentId.h"

namespace Component {

    class Dirty {

        inline static Component::ComponentId m_id = Component::ComponentId(true);

    public:
        static Component::ComponentId id() { return m_id; }
    };

}

#endif //ENGINE_DIRTY_H
