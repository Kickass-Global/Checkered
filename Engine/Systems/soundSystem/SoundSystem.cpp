//
// Created by root on 29/2/20.
//

#include <EngineDebug.hpp>
#include "SoundSystem.h"
#include "Sound.h"



void Engine::SoundSystem::initialize() {
   
    onKeyPressHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(this, &Engine::SoundSystem::onKeyPress);
    onKeyDownHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(this, &Engine::SoundSystem::onKeyDown);
    onKeyUpHandler = getEngine()->getSubSystem<EventSystem>()->createHandler(this, &Engine::SoundSystem::onKeyUp);


    ALCdevice* openALDevice = alcOpenDevice(nullptr);
    if (!openALDevice)
        std::cerr << "Error could not open device";

    
    
    if (!alcCall(alcCreateContext, openALContext, openALDevice, openALDevice, nullptr) || !openALContext)
    {
        std::cerr << "ERROR: Could not create audio context" << std::endl;
        
    }
    ALCboolean contextMadeCurrent = false;
    if (!alcCall(alcMakeContextCurrent, contextMadeCurrent, openALDevice, openALContext)
        || contextMadeCurrent != ALC_TRUE)
    {
        std::cerr << "ERROR: Could not make audio context current" << std::endl;
        
    }
    sourceHorn = load_sound("Assets/Sounds/CARHORN4.wav");
    sourceAcceleration = load_looping_sound("Assets/Sounds/car+geardown.wav");
    sourceBreaking = load_sound("Assets/Sounds/TIRE+SKID.wav");
    sourceMusic = load_looping_sound("Assets/Sounds/backGroundMusic.wav");
    sourceDriving = load_looping_sound("Assets/Sounds/carMoving.wav");
    sourceCollision = load_sound("Assets/Sounds/CarCrash.wav");
    sourcePassengerDlivered = load_sound("Assets/Sounds/PassengerSuccess.wav");

    playSound(sourceMusic);

    accelerationBuffer = load_buffer("Assets/Sounds/car+geardown.wav");
    breakingBuffer = load_buffer("Assets/Sounds/TIRE+SKID.wav");
    drivingBuffer = load_buffer("Assets/Sounds/carMoving.wav");
}

void Engine::SoundSystem::update(Engine::deltaTime) {
    /*


    auto sounds = getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Sound>();
    for (auto sound : sounds)
    {
        if (sound->name == "horn")
        {
            Engine::log<module, Engine::high>("Playing sound ", sound->name);
            playSound(sourceHorn);
            getEngine()->getSubSystem<EngineStore>()->getRoot().eraseComponent<Component::Sound>(sound);
        }
     */
    auto sounds = getEngine()->getSubSystem<EngineStore>()->getRoot().getComponentsOfType<Component::Sound>();
    for (auto sound : sounds)
    {
        if (sound->name == "acceleration")
        {
            Engine::log<module, Engine::high>("Playing sound ", sound->name);
            playSound(sourceAcceleration);
            getEngine()->getSubSystem<EngineStore>()->getRoot().eraseComponent<Component::Sound>(sound);
        }
        else if (sound->name == "breaking")
        {
            Engine::log<module, Engine::high>("Playing sound ", sound->name);
            playSound(sourceBreaking);getEngine()->getSubSystem<EngineStore>()->getRoot().eraseComponent<Component::Sound>(sound);
        }
        else if (sound->name == "stopAcceleration")
        {
            Engine::log<module, Engine::high>("Playing sound ", sound->name);
            stopSound(sourceAcceleration);getEngine()->getSubSystem<EngineStore>()->getRoot().eraseComponent<Component::Sound>(sound);
        }
        else if (sound->name == "stopBreaking")
        {
            Engine::log<module, Engine::high>("Playing sound ", sound->name);
            stopSound(sourceBreaking);getEngine()->getSubSystem<EngineStore>()->getRoot().eraseComponent<Component::Sound>(sound);
        }
        else if (sound->name == "passengerDroppedOff")
        {
            Engine::log<module, Engine::high>("Playing sound ", sound->name);
            playSound(sourcePassengerDlivered);getEngine()->getSubSystem<EngineStore>()->getRoot().eraseComponent<Component::Sound>(sound);
        }
        else if (sound->name == "damageRecieved")
        {
            Engine::log<module, Engine::high>("Playing sound ", sound->name);
            playSound(sourceCollision); getEngine()->getSubSystem<EngineStore>()->getRoot().eraseComponent<Component::Sound>(sound);
        }
    
    }
}




std::int32_t Engine::SoundSystem::convert_to_int(char* buffer, std::size_t len)
{
    std::int32_t a = 0;
    //if (std::endian::native == std::endian::little)
    std::memcpy(&a, buffer, len);
    // else
      //  for (std::size_t i = 0; i < len; ++i)
       //    reinterpret_cast<char*>(&a)[3 - i] = buffer[i];
    return a;
}

bool Engine::SoundSystem::load_wav_file_header(std::ifstream& file,
    std::uint8_t& channels,
    std::int32_t& sampleRate,
    std::uint8_t& bitsPerSample,
    ALsizei& size)
{
    char buffer[4];
    if (!file.is_open())
        return false;

    // the RIFF
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read RIFF" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "RIFF", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't begin with RIFF)" << std::endl;
        return false;
    }

    // the size of the file
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read size of file" << std::endl;
        return false;
    }

    // the WAVE
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read WAVE" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "WAVE", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (header doesn't contain WAVE)" << std::endl;
        return false;
    }

    // "fmt/0"
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read fmt/0" << std::endl;
        return false;
    }

    // this is always 16, the size of the fmt data chunk
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read the 16" << std::endl;
        return false;
    }

    // PCM should be 1?
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read PCM" << std::endl;
        return false;
    }

    // the number of channels
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read number of channels" << std::endl;
        return false;
    }
    channels = convert_to_int(buffer, 2);

    // sample rate
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read sample rate" << std::endl;
        return false;
    }
    sampleRate = convert_to_int(buffer, 4);

    // (sampleRate * bitsPerSample * channels) / 8
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read (sampleRate * bitsPerSample * channels) / 8" << std::endl;
        return false;
    }

    // ?? dafaq
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read dafaq" << std::endl;
        return false;
    }

    // bitsPerSample
    if (!file.read(buffer, 2))
    {
        std::cerr << "ERROR: could not read bits per sample" << std::endl;
        return false;
    }
    bitsPerSample = convert_to_int(buffer, 2);

    // data chunk header "data"
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data chunk header" << std::endl;
        return false;
    }
    if (std::strncmp(buffer, "data", 4) != 0)
    {
        std::cerr << "ERROR: file is not a valid WAVE file (doesn't have 'data' tag)" << std::endl;
        return false;
    }

    // size of data
    if (!file.read(buffer, 4))
    {
        std::cerr << "ERROR: could not read data size" << std::endl;
        return false;
    }
    size = convert_to_int(buffer, 4);

    /* cannot be at the end of file */
    if (file.eof())
    {
        std::cerr << "ERROR: reached EOF on the file" << std::endl;
        return false;
    }
    if (file.fail())
    {
        std::cerr << "ERROR: fail state set on the file" << std::endl;
        return false;
    }

    return true;
}

char* Engine::SoundSystem::load_wav(const std::string& filename,
    std::uint8_t& channels,
    std::int32_t& sampleRate,
    std::uint8_t& bitsPerSample,
    ALsizei& size)
{
    std::ifstream in(filename, std::ios::binary);
    if (!in.is_open())
    {
        std::cerr << "ERROR: Could not open \"" << filename << "\"" << std::endl;
        return nullptr;
    }
    
    if (!load_wav_file_header(in, channels, sampleRate, bitsPerSample, size))
    {
        std::cerr << "ERROR: Could not load wav header of \"" << filename << "\"" << std::endl;
        return nullptr;
    }

    char* data = new char[size];

    in.read(data, size);

    return data;
}
ALuint Engine::SoundSystem::load_sound(std::string filePath)
{
    
        std::uint8_t 	channels;
        std::int32_t 	sampleRate;
        std::uint8_t 	bitsPerSample;
        ALsizei			dataSize;
        char* rawSoundData = load_wav(filePath, channels, sampleRate, bitsPerSample, dataSize);
        if (rawSoundData == nullptr || dataSize == 0)
        {
            std::cerr << "ERROR: Could not load wav" << std::endl;
            
        }
        std::vector<char> soundData(rawSoundData, rawSoundData + dataSize);

        ALuint buffer;
        alCall(alGenBuffers, 1, &buffer);

        ALenum format;
        if (channels == 1 && bitsPerSample == 8)
            format = AL_FORMAT_MONO8;
        else if (channels == 1 && bitsPerSample == 16)
            format = AL_FORMAT_MONO16;
        else if (channels == 2 && bitsPerSample == 8)
            format = AL_FORMAT_STEREO8;
        else if (channels == 2 && bitsPerSample == 16)
            format = AL_FORMAT_STEREO16;
        else
        {
            std::cerr
                << "ERROR: unrecognised wave format: "
                << channels << " channels, "
                << bitsPerSample << " bps" << std::endl;
            
        }
        
        alCall(alBufferData, buffer, format, soundData.data(), soundData.size(), sampleRate);
        soundData.clear(); // erase the sound in RAM


        ALuint source;
        
        alCall(alGenSources, 1, &source);
        alCall(alSourcef, source, AL_PITCH, 1);
        alCall(alSourcef, source, AL_GAIN, 1.0f);
        alCall(alSource3f, source, AL_POSITION, 0, 0, 0);
        alCall(alSource3f, source, AL_VELOCITY, 0, 0, 0);
        alCall(alSourcei, source, AL_LOOPING, AL_FALSE);
        alCall(alSourcei, source, AL_BUFFER, buffer);
        
        return source;
}

ALuint Engine::SoundSystem::load_looping_sound(std::string filePath)
{

    std::uint8_t 	channels;
    std::int32_t 	sampleRate;
    std::uint8_t 	bitsPerSample;
    ALsizei			dataSize;
    char* rawSoundData = load_wav(filePath, channels, sampleRate, bitsPerSample, dataSize);
    if (rawSoundData == nullptr || dataSize == 0)
    {
        std::cerr << "ERROR: Could not load wav" << std::endl;

    }
    std::vector<char> soundData(rawSoundData, rawSoundData + dataSize);

    ALuint buffer;
    alCall(alGenBuffers, 1, &buffer);

    ALenum format;
    if (channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
    {
        std::cerr
            << "ERROR: unrecognised wave format: "
            << channels << " channels, "
            << bitsPerSample << " bps" << std::endl;

    }

    alCall(alBufferData, buffer, format, soundData.data(), soundData.size(), sampleRate);
    soundData.clear(); // erase the sound in RAM


    ALuint source;

    alCall(alGenSources, 1, &source);
    alCall(alSourcef, source, AL_PITCH, 1);
    alCall(alSourcef, source, AL_GAIN, volume);
    alCall(alSource3f, source, AL_POSITION, 0, 0, 0);
    alCall(alSource3f, source, AL_VELOCITY, 0, 0, 0);
    alCall(alSourcei, source, AL_LOOPING, AL_TRUE);
    alCall(alSourcei, source, AL_BUFFER, buffer);

    return source;
}

int Engine::SoundSystem::playSound(ALuint s)
{
        alCall(alSourcePlay, s);


        return 0;
       
}

int Engine::SoundSystem::stopSound(ALuint s)
{
    alCall(alSourceStop, s);
    return 0;
}


void Engine::SoundSystem::volumeDown()
{
    volume = volume - 0.1;
    alCall(alSourcef, sourceMusic, AL_GAIN, volume);
    alCall(alSourcef, sourceAcceleration, AL_GAIN, volume);
    alCall(alSourcef, sourceBreaking, AL_GAIN, volume);
    alCall(alSourcef, sourceHorn, AL_GAIN, volume);

}

void Engine::SoundSystem::volumeUp()
{
    volume = volume + 0.1;
    alCall(alSourcef, sourceMusic, AL_GAIN, volume);
    alCall(alSourcef, sourceAcceleration, AL_GAIN, volume);
    alCall(alSourcef, sourceBreaking, AL_GAIN, volume);
    alCall(alSourcef, sourceHorn, AL_GAIN, volume);
}










void Engine::SoundSystem::onKeyDown(const Component::EventArgs<int>& args)
{



    /*



    auto key = args.get<0>();
    if (key == GLFW_KEY_W)
    {
        playSound(sourceAcceleration);
    }



    else if (key == GLFW_KEY_S)
    {
        playSound(sourceBreaking);

    }
     */

}
void  Engine::SoundSystem::onKeyUp(const Component::EventArgs<int>& args)
{

    /*

    auto key = args.get<0>();
    if (key == GLFW_KEY_W)
    {
        stopSound(sourceAcceleration);
    }


    else if (key == GLFW_KEY_S)
    {
        stopSound(sourceBreaking);
    }

    */
}
void Engine::SoundSystem::onKeyPress(const Component::EventArgs<int>& args)
{
    auto key = args.get<0>();
    if (key == GLFW_KEY_SPACE)
    {
        playSound(sourceHorn);
    }
    else if (key == GLFW_KEY_F1)
    {
        volumeUp();
    }
    else if (key == GLFW_KEY_F2)
    {
        volumeDown();
    }
}

ALuint Engine::SoundSystem::load_buffer(std::string filePath)
{
    std::uint8_t 	channels;
    std::int32_t 	sampleRate;
    std::uint8_t 	bitsPerSample;
    ALsizei			dataSize;
    char* rawSoundData = load_wav(filePath, channels, sampleRate, bitsPerSample, dataSize);
    if (rawSoundData == nullptr || dataSize == 0)
    {
        std::cerr << "ERROR: Could not load wav" << std::endl;

    }
    std::vector<char> soundData(rawSoundData, rawSoundData + dataSize);

    ALuint buffer;
    alCall(alGenBuffers, 1, &buffer);

    ALenum format;
    if (channels == 1 && bitsPerSample == 8)
        format = AL_FORMAT_MONO8;
    else if (channels == 1 && bitsPerSample == 16)
        format = AL_FORMAT_MONO16;
    else if (channels == 2 && bitsPerSample == 8)
        format = AL_FORMAT_STEREO8;
    else if (channels == 2 && bitsPerSample == 16)
        format = AL_FORMAT_STEREO16;
    else
    {
        std::cerr
            << "ERROR: unrecognised wave format: "
            << channels << " channels, "
            << bitsPerSample << " bps" << std::endl;

    }

    alCall(alBufferData, buffer, format, soundData.data(), soundData.size(), sampleRate);
    soundData.clear(); // erase the sound in RAM

    return buffer;
}
