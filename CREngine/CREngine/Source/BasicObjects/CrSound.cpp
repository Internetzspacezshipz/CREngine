#include "CrSound.h"
#include "CrSerialization.h"
#include "CrGlobals.h"

REGISTER_CLASS_FLAGS(CrSound, CrClassFlags_Unique);

REGISTER_EXTENSION(CrSound, ".crsnd");

CrSound::~CrSound()
{
	//Simply reduces the reference counter in the mini audio manager.
	UnloadSound();
}

void CrSound::BinSerialize(CrArchive& Arch)
{
	Arch <=> ImportPath;

	if (Arch.bSerializing)
	{
		Import();
	}

	Arch <=> Settings;
	Arch <=> RawSound;

	//Load audio.
	if (!Arch.bSerializing)
	{
		LoadSound();
	}
}

bool CrSound::Import()
{
	// (DELETE RAW SOUND HERE TO MAKE SURE IT IS EMPTY BEFORE IMPORT)

	auto FileStr = (BasePath() / ImportPath).string();



	return RawSound.size();
}

bool CrSound::LoadSound()
{
	//bInitialized = AudioSystem->ImportAudio(&Sound);
	return bInitialized;
}

void CrSound::UnloadSound()
{
	if (bInitialized)
	{
		//AudioSystem->DestroySound(&Sound, Decoder);
		bInitialized = false;
	}
}

void CrSound::UpdateSettings()
{
	if (bInitialized)
	{
		//AudioSystem->SetSoundSettings(&Sound, Settings);
	}
}

bool CrSound::Play()
{
	if (bInitialized)
	{
		//return AudioSystem->PlayAudio(&Sound);
	}
	return false;
}

bool CrSound::Stop()
{
	if (bInitialized)
	{
		//return AudioSystem->StopAudio(&Sound);
	}
	return false;
}

float CrSound::GetDuration()
{
	return 0.0f;
}

void CrSound::Construct()
{
	 AudioSystem = CrGlobals::GetAudioSystemPointer();
}
