#include "CrAudioSystem.h"
#include "CrPaths.h"


#define CHANNELS    2
#define SAMPLE_RATE 48000

bool CrAudioSystem::InitializeAudioEngine()
{
    return AudioEngine.init() == 0;
}

SoLoud::handle CrAudioSystem::PlaySound(SoLoud::Wav& SoundToPlay)
{
    return AudioEngine.play(SoundToPlay);
}

void CrAudioSystem::StopSound(SoLoud::handle HandleToStop)
{
    AudioEngine.stop(HandleToStop);
}

void CrAudioSystem::StopAll(SoLoud::Wav& SoundToPlay)
{
    AudioEngine.stopAudioSource(SoundToPlay);
}

void CrAudioSystem::SetSoundSettings(SoLoud::handle Handle, const CrSoundSetting& InSettings)
{
    AudioEngine.setVolume(Handle, InSettings.Volume);
    AudioEngine.setLooping(Handle, InSettings.bLooping);
}

void CrAudioSystem::DestroyAudioEngine()
{
    AudioEngine.deinit();
}
