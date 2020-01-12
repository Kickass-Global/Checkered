//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INPUTSYSTEM_H
#define ENGINE_INPUTSYSTEM_H

#include "GLFW/glfw3.h"
#include "../../SystemCalls.h"
#include <memory>
#include <functional>

namespace Input {

    constexpr char module[] = "Input";

    template<typename... Args>
    class Event {
        std::vector<std::function<void(Args...)>> handlers;
    public:
        void operator()(Args &... args) {
            for (auto &&handler : handlers) {
                if (handler) handler(std::forward<Args>(args)...);
            }
        }

        void operator+=(std::function<void(Args...)> handler) {
            handlers.push_back(handler);
        }
    };

    class InputSystem {

        static void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods) {
            if (action == GLFW_RELEASE) {
                system_calls::log<module>("onKeyPress: ", key);
                onKeyPress(key);
            }
        }

    public:
        inline static Event<int> onKeyPress;

        static void initialize(GLFWwindow* window) {
            glfwSetKeyCallback(window, keyHandler);
        }

    };

}


#endif //ENGINE_INPUTSYSTEM_H
