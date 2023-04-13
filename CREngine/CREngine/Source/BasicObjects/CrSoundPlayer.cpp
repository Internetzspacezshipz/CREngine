#include "CrSoundPlayer.h"
#include "CrSerialization.h"
#include "CrGlobals.h"

REGISTER_CLASS_FLAGS(CrSoundPlayer, CrClassFlags_Component);

CrSoundPlayer::CrSoundPlayer()
{
	AudioSystem = CrGlobals::GetAudioSystemPointer();
}

CrSoundPlayer::~CrSoundPlayer()
{
	//Simply reduces the reference counter in the mini audio manager.
	UnloadSound();
}

void CrSoundPlayer::BinSerialize(CrArchive& Arch)
{
	Arch <=> Sound;
	Arch <=> Settings;

	//Load audio.
	if (!Arch.bSerializing)
	{
		LoadSound();
	}
}

bool CrSoundPlayer::LoadSound()
{
	if (Sound.IsLoadedOrLoadable())
	{
		if (false == Sound.IsLoaded())
		{
			Sound.Load();
		}
		return true;
	}
	return false;
}

void CrSoundPlayer::UnloadSound()
{

}

void CrSoundPlayer::UpdateSettings()
{

}

bool CrSoundPlayer::Play()
{
	if (Sound.IsLoaded())
	{
		SoundHandle = AudioSystem->PlaySound(Sound->WaveObject);
		return true;
	}
	return false;
}

bool CrSoundPlayer::Stop()
{
	if (SoundHandle)
	{
		AudioSystem->StopSound(SoundHandle);
		return true;
	}
	return false;
}

float CrSoundPlayer::GetDuration()
{
	return Sound->WaveObject.getLength();
}
