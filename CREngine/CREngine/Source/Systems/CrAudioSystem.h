#pragma once
#include <CrSerializationBin.h>
#include "soloud.h"
#include "soloud_wav.h"

//Settings for an audio play event.
struct CrSoundSetting
{
	bool bLooping = false;
	float Volume = 1.f;
	float StartTime = 0.f;
	float EndTime = FLT_MAX;
};

__forceinline static void operator <=>(CrArchive& Arch, CrSoundSetting& ToSerialize)
{
	Arch <=> ToSerialize.bLooping;
	Arch <=> ToSerialize.Volume;
	Arch <=> ToSerialize.StartTime;
	Arch <=> ToSerialize.EndTime;
}

//UNTESTED
class CrAudioSystem
{
	SoLoud::Soloud AudioEngine;
	SoLoud::Wav TestWav;

	uint32_t SDL_DeviceID;

public:
	//Sets up the audio system
	bool InitializeAudioEngine();

	//Make a sound available to play using PlayAudio
	bool ImportAudio(BinArray& DataOut, const Path& TargetPath);

	//Play an audio that has been made available via InitializeSound
	bool PlayAudio(SoLoud::Wav& OutData);

	//Stop a playing audio file
	bool StopAudio();

	//Destroys a sound and makes it no longer valid to call PlayAudio for it.
	void DestroySound();

	//Sound Settings
	void SetSoundSettings(const CrSoundSetting& InSettings);

	//Destroys the audio system
	void DestroyAudioEngine();

	SoLoud::Soloud* GetEngine() { return &AudioEngine; };
};

