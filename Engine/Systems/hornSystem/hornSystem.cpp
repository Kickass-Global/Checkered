#pragma comment(lib, "Winmm.lib")

#include <windows.h>
#include <Engine.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <mmsystem.h>

#include "hornSystem.h"

using namespace std;

void Horn::hornSystem::initialize()
{
	onKeyPressHandler = Engine::EventSystem::createHandler(this, &Horn::hornSystem::onKeyPress);
	onKeyDownHandler = Engine::EventSystem::createHandler(this, &Horn::hornSystem::onKeyDown);
	onKeyUpHandler = Engine::EventSystem::createHandler(this, &Horn::hornSystem::onKeyUp);

	cout << "hornSystem initialized" << endl;
}


//std::set<int> keys1;
void Horn::hornSystem::update(Engine::deltaTime elapsed) {
    
}

void Horn::hornSystem::onKeyDown(const Component::EventArgs<int>& args) {
    //Engine::log<module, Engine::low>("onKeyDown=", std::get<0>(args.values));
   
   
}

void  Horn::hornSystem::onKeyUp(const Component::EventArgs<int>& args) {
    
}

void Horn::hornSystem::onKeyPress(const Component::EventArgs<int>& args) {
     Engine::log<module, Engine::low>("onKeyPress=", std::get<0>(args.values));
 
     auto key = std::get<0>(args.values);
         if (key == GLFW_KEY_SPACE)
         {
             bool played = PlaySound("carHorn.wav", NULL, SND_ASYNC | SND_FILENAME);
             cout << "Success or not: " << played << endl;
         }

}