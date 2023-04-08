#include "CrSoundPlayer.h"
#include "CrSerialization.h"
#include "CrGlobals.h"

REGISTER_CLASS_FLAGS(CrSoundPlayer, CrClassFlags_Component);

CrSoundPlayer::~CrSoundPlayer()
{
	//Simply reduces the reference counter in the mini audio manager.
	UnloadSound();
}

void CrSoundPlayer::BinSerialize(CrArchive& Arch)
{
	//Arch <=> AudioFile;
	Arch <=> Settings;

	//Load audio.
	if (!Arch.bSerializing)
	{
		LoadSound();
	}
}

bool CrSoundPlayer::LoadSound()
{
	//bInitialized = AudioSystem->ImportAudio(&Sound, Decoder, AudioFile);
	//return bInitialized;
	return true;
}

void CrSoundPlayer::UnloadSound()
{
	//if (bInitialized)
	//{
	//	AudioSystem->DestroySound(&Sound, Decoder);
	//	bInitialized = false;
	//}
}

void CrSoundPlayer::UpdateSettings()
{
	//if (bInitialized)
	{
	//	AudioSystem->SetSoundSettings(&Sound, Settings);
	}
}

bool CrSoundPlayer::Play()
{
	//if (bInitialized)
	{
	//	return AudioSystem->PlayAudio(&Sound);
	}
	return false;
}

bool CrSoundPlayer::Stop()
{
	//if (bInitialized)
	{
	//	return AudioSystem->StopAudio(&Sound);
	}
	return false;
}

float CrSoundPlayer::GetDuration()
{
	return 0.0f;
}

void CrSoundPlayer::Construct()
{
	AudioSystem = CrGlobals::GetAudioSystemPointer();
}
