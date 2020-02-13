//
// Created by root on 11/1/20.
//

#include "../../Components/Component.h"
#include "InputSystem.h"
#include "Engine.h"

Component::ComponentEvent<int> Input::InputSystem::onKeyPress("onKeyPress");
Component::ComponentEvent<int> Input::InputSystem::onKeyDown("onKeyDown");
Component::ComponentEvent<int> Input::InputSystem::onKeyUp("onKeyUp");

void Input::InputSystem::initialize(GLFWwindow *window) {
    glfwSetKeyCallback(window, keyHandler);
}

void Input::InputSystem::keyHandler(GLFWwindow *, int key, int, int action, int) {

    if (action == GLFW_RELEASE) {
        Engine::log<module>("onKeyPress: ", key);
        onKeyPress(key);
        onKeyUp(key);
    }
    if (action == GLFW_PRESS) {
        Engine::log<module, Engine::low>("onKeyDown: ", key);
        onKeyDown(key);
    }
}

void Input::InputSystem::update(Engine::deltaTime) {}
