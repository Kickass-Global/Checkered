#pragma once



#include "ComponentId.h"
#include "Camera.h"
#include "systeminterface.hpp"
#include "EventHandler.h"
#include "Engine.h"
#include "Events/Events.h"
#include <iostream>
#include "soundSystem/sound.h"

#include <windows.h>
#include <mmsystem.h>
#include <GLFW\glfw3.h>


namespace Horn {

    const char module[] = "Horn";

    class hornSystem : public Engine::SystemInterface {

    public:
        void initialize() override;
        void update(Engine::deltaTime /*elapsed*/) override;

    //    Component::ComponentId onKeyPressHandler;
    //    Component::ComponentId onKeyDownHandler;
    //    Component::ComponentId onKeyUpHandler;

    //    //std::set<int> keys;
    //private:
    //    //std::set<int> keys;
    //    void onKeyDown(const Component::EventArgs<int>& args);
    //    void onKeyUp(const Component::EventArgs<int>& args);
    //    void onKeyPress(const Component::EventArgs<int>& args);
    //    void sound();
    };
}
