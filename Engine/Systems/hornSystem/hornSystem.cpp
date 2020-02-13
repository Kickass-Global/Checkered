
#include <windows.h>
#include <Engine.h>
#include <GLFW\glfw3.h>

#include "hornSystem.h"

void Horn::hornSystem::initialize()
{
	onKeyPressHandler = Engine::EventSystem::createHandler(this, &Horn::hornSystem::onKeyPress);
	onKeyDownHandler = Engine::EventSystem::createHandler(this, &Horn::hornSystem::onKeyDown);
	onKeyUpHandler = Engine::EventSystem::createHandler(this, &Horn::hornSystem::onKeyUp);

	std::cout << "hornSystem initialized" << std::endl;
}

void Horn::hornSystem::update(Engine::deltaTime /*elapsed*/)
{
	//do nothing
}

void Horn::hornSystem::onKeyDown(const Component::EventArgs<int>& args) {
	Engine::log<module, Engine::Importance::low>("onKeyDown=", std::get<0>(args.values));

	auto key = std::get<0>(args.values);



	if (key == GLFW_KEY_SPACE) {

		//bool played = PlaySound("carHorn.wav", NULL, SND_SYNC);

	}
}

void Horn::hornSystem::onKeyUp(const Component::EventArgs<int>& args) {}
void Horn::hornSystem::onKeyPress(const Component::EventArgs<int>& args) {}