//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#ifndef ENGINE_SYSTEMINTERFACE_HPP
#define ENGINE_SYSTEMINTERFACE_HPP

#include "../SystemCalls.h"

namespace Engine {

    typedef float deltaTime;

    class SystemInterface {
    public:
        virtual void update(Engine::deltaTime /*elapsed*/) = 0;

        virtual void initialize() {
            Engine::log<module>("Initializing system");
        }
    };

}

#endif //ENGINE_SYSTEMINTERFACE_HPP
