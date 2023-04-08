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
	if (LoadableSound.IsLoadedOrLoadable())
	{
		if (false == LoadableSound.IsLoaded())
		{
			LoadableSound.Load();
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
	if (LoadableSound.IsLoaded())
	{
		SoundHandle = AudioSystem->GetEngine()->play(LoadableSound->WaveObject);
		return true;
	}
	return false;
}

bool CrSoundPlayer::Stop()
{
	if (SoundHandle)
	{
		AudioSystem->GetEngine()->stop(SoundHandle);
		return true;
	}
	return false;
}

float CrSoundPlayer::GetDuration()
{
	return LoadableSound->WaveObject.getLength();
}

void CrSoundPlayer::Construct()
{
	AudioSystem = CrGlobals::GetAudioSystemPointer();
}
