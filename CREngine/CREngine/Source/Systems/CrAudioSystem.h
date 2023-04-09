#pragma once
#include <CrSerializationBin.h>
#include "soloud.h"
#include "soloud_wav.h"

//Settings for an audio play event.
struct CrSoundSetting
{
	bool bLooping = false;
	float Volume = 1.f;
};

__forceinline static void operator <=>(CrArchive& Arch, CrSoundSetting& ToSerialize)
{
	Arch <=> ToSerialize.bLooping;
	Arch <=> ToSerialize.Volume;
}

//UNTESTED
class CrAudioSystem
{
	SoLoud::Soloud AudioEngine;

public:
	//Sets up the audio system
	bool InitializeAudioEngine();

	SoLoud::handle PlaySound(SoLoud::Wav& SoundToPlay);
	void StopSound(SoLoud::handle HandleToStop);

	//Called at the end of a CrSound's lifespan to make sure there are no instances of it being used.
	void StopAll(SoLoud::Wav& SoundToPlay);

	//Sound Settings
	void SetSoundSettings(SoLoud::handle Handle, const CrSoundSetting& InSettings);

	//Destroys the audio system
	void DestroyAudioEngine();
};

