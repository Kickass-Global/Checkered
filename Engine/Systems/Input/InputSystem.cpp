//
// Created by root on 11/1/20.
//

#include "../../Components/Component.h"
#include "InputSystem.h"
#include "Engine.h"

Component::EventDelegate<int> Input::InputSystem::onKeyPress("onKeyPress");
Component::EventDelegate<int> Input::InputSystem::onKeyDown("onKeyDown");
Component::EventDelegate<int> Input::InputSystem::onKeyUp("onKeyUp");
Component::EventDelegate<GLFWgamepadstate, GLFWgamepadstate> Input::InputSystem::onGamePadStateChanged("onGamePadStateChanged");

void Input::InputSystem::initialize(GLFWwindow *window) {
	glfwSetKeyCallback(window, keyHandler);
	glfwSetJoystickCallback(gamepadHandler);


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

void Input::InputSystem::gamepadHandler(int jid, int event) {

	if (event == GLFW_CONNECTED)
	{
		Engine::log<module, Engine::high>("gamepad connected ", jid);
	}
	else if (event == GLFW_DISCONNECTED)
	{
		Engine::log<module, Engine::high>("gamepad disconnected ", jid);
	}
}

bool operator == (const GLFWgamepadstate a, const GLFWgamepadstate b)
{
	for (int i = 0; i < sizeof(a.buttons) / sizeof(a.buttons[0]); ++i)
	{
		if (a.buttons[i] != b.buttons[i]) return false;
	}

	for (int i = 0; i < sizeof(a.axes) / sizeof(a.axes[0]); ++i)
	{
		if (a.axes[i] != b.axes[i]) return false;
	}

	return true;
}


bool operator != (const GLFWgamepadstate a, const GLFWgamepadstate b)
{
	return !(a==b);
}

void Input::InputSystem::update(Engine::deltaTime) {
	static GLFWgamepadstate current, previous;
	previous = current;

	if (glfwGetGamepadState(GLFW_JOYSTICK_1, &current))
	{
		if (current != previous)
			onGamePadStateChanged(previous, current);
	}

}
