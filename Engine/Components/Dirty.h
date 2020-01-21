//
// Created by root on 11/1/20.
//

#ifndef ENGINE_DIRTY_H
#define ENGINE_DIRTY_H

#include "ComponentId.h"
#include "ComponentBase.h"

namespace Component {

    class Dirty {
    public:
        static ComponentId id() { return {true, 0xAAAA0001};}
    };

}

#endif //ENGINE_DIRTY_H
