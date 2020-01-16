//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INPUTSYSTEM_H
#define ENGINE_INPUTSYSTEM_H

#include "GLFW/glfw3.h"
#include "../../Engine.h"
#include "../../SystemCalls.h"
#include <memory>
#include <functional>

namespace Input {

    constexpr char module[] = "Input";

    class InputSystem {

        static void keyHandler(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/) {

            if (action == GLFW_RELEASE) {
                Engine::log<module, Engine::Importance::low>("onKeyPress: ", key);
                onKeyPress(key);
                onKeyUp(key);
            }
            if (action == GLFW_PRESS)
            {
                Engine::log<module, Engine::Importance::low>("onKeyDown: ", key);
                onKeyDown(key);
            }
        }

    public:
        inline static Engine::Event<int> onKeyPress;
        inline static Engine::Event<int> onKeyDown;
        inline static Engine::Event<int> onKeyUp;

        static void initialize(GLFWwindow* window) {
            glfwSetKeyCallback(window, keyHandler);
        }

    };

}


#endif //ENGINE_INPUTSYSTEM_H
