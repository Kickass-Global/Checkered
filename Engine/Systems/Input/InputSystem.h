//
// Created by root on 11/1/20.
//

#ifndef ENGINE_INPUTSYSTEM_H
#define ENGINE_INPUTSYSTEM_H

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "../../main.h"
#include "../../SystemCalls.h"
#include "../../Components/ComponentEvent.h"
#include <memory>
#include <functional>

namespace Input {

    static char module[] = "Input";

    class InputSystem {

        static void keyHandler(GLFWwindow* /*window*/, int key, int /*scancode*/, int action, int /*mods*/);

    public:

        static Component::ComponentEvent<int> onKeyPress;
        static Component::ComponentEvent<int> onKeyDown;
        static Component::ComponentEvent<int> onKeyUp;

        static void initialize(GLFWwindow* window);

    };

}


#endif //ENGINE_INPUTSYSTEM_H
