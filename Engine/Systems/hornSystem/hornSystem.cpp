#pragma comment(lib, "winmm.lib")

#include <iostream>
#include <string>
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
   /* auto key = std::get<0>(args.values);
    if (keys.count(GLFW_KEY_SPACE))
    {
        sound();

    }
    */
 }

void Horn::hornSystem::onKeyDown(const Component::EventArgs<int>& args) {
    Engine::log<module, Engine::Importance::low>("onKeyDown=", std::get<0>(args.values));
   /*
    auto key = std::get<0>(args.values);

    keys.emplace(key);
   */
}

void  Horn::hornSystem::onKeyUp(const Component::EventArgs<int>& args) {
    Engine::log<module, Engine::Importance::low>("onKeyUp=", std::get<0>(args.values));
    /*
    auto key = std::get<0>(args.values);
    keys.erase(key);
    */
    }

void Horn::hornSystem::onKeyPress(const Component::EventArgs<int>& args) {
     Engine::log<module, Engine::low>("onKeyPress=", std::get<0>(args.values));
 
     auto key = std::get<0>(args.values);
     if (key == GLFW_KEY_SPACE)
     {

     }

}
/*
void Horn::hornSystem::sound()
{ 
   PlaySound("Assets/Sounds/carHorn.wav", NULL, SND_ASYNC | SND_FILENAME);
   // PlaySound("D:\\carHorn.wav", NULL, SND_ASYNC | SND_FILENAME);
    //system("pause");
}

*/