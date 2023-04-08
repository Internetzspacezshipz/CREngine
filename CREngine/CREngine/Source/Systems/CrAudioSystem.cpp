#include "CrAudioSystem.h"
#include "CrPaths.h"


#define CHANNELS    2
#define SAMPLE_RATE 48000

template<typename Type>
Type StoMS(const Type& Value)
{
    return Value * (Type)1000;
}

bool CrAudioSystem::InitializeAudioEngine()
{
    return AudioEngine.init() == 0;
}

bool CrAudioSystem::StopAudio()
{
    return false;
}

void CrAudioSystem::DestroySound()
{
}

void CrAudioSystem::SetSoundSettings(const CrSoundSetting& InSettings)
{
}

void CrAudioSystem::DestroyAudioEngine()
{
    AudioEngine.deinit();
}
