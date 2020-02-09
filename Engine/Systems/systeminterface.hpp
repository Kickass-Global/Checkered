//
// Created by Jackson Cougar Wiebe on 1/27/2020.
//

#ifndef ENGINE_SYSTEMINTERFACE_HPP
#define ENGINE_SYSTEMINTERFACE_HPP


namespace Engine {

    typedef float deltaTime;

    class SystemInterface {
    public:
        virtual void update(Engine::deltaTime /*elapsed*/) = 0;

        virtual void initialize() {
        }
    };

}

#endif //ENGINE_SYSTEMINTERFACE_HPP
