//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INPUTSYSTEM_H
#define ENGINE_INPUTSYSTEM_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../../main.h"
#include "EventDelegate.h"
#include "../../Systems/systeminterface.hpp"
#include <memory>
#include <functional>

namespace Input {

    namespace {
        char module[] = "Input";
    }

    class InputSystem : public Engine::SystemInterface {

        static void
        keyHandler(GLFWwindow * /*window*/, int key, int /*scancode*/,
                   int action, int /*mods*/);

        static void
            gamepadHandler(int jid, int action);

    public:

        static Component::EventDelegate<int> onKeyPress;
        static Component::EventDelegate<int> onKeyDown;
        static Component::EventDelegate<int> onKeyUp;

        void initialize(GLFWwindow *window);

        void update(Engine::deltaTime /*elapsed*/);

    };

}


#endif //ENGINE_INPUTSYSTEM_H
