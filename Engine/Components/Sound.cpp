#include "Sound.h"

Component::Sound::Sound(std::string s)
{
	 name = s;
}

std::string  Component::Sound::getSoundName()
{
	return name;
}