//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#pragma once

#ifndef ENGINE_SYSTEMINTERFACE_HPP
#define ENGINE_SYSTEMINTERFACE_HPP


namespace Engine {

    typedef float deltaTime;

    class SystemInterface {
    public:
        int order = 1;
        virtual void update(Engine::deltaTime /*elapsed*/) = 0;

        virtual void initialize() {
        }
    };

}

#endif //ENGINE_SYSTEMINTERFACE_HPP
