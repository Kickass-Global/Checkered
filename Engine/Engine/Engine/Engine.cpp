// Engine.cpp : Defines the entry point for the application.
//

#include "Systems/RenderingSystem.h"

using namespace std;

int main()
{
    RenderingSystem RS;
    RS.initialize();

    while(true)
    {
        RS.update();
    }
}
