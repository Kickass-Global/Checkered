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

    public:

        static Component::ComponentEvent<int> onKeyPress;
        static Component::ComponentEvent<int> onKeyDown;
        static Component::ComponentEvent<int> onKeyUp;

        void initialize(GLFWwindow *window);

        void update(Engine::deltaTime /*elapsed*/);

    };

}


#endif //ENGINE_INPUTSYSTEM_H
