#include "SoundSystem.h"

using namespace NCL;
using namespace CSC8503;


SoundSystem::SoundSystem()
{
    static unsigned char key = 0;

    InitAudioDevice();

    Sound fxWav = LoadSound("../Assets/Sound/warning.wav");

    PlaySound(fxWav);
}

SoundSystem::~SoundSystem()
{
    CloseAudioDevice();
}

void SoundSystem::Update(float dt)
{

}