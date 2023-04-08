#pragma once

#include "CrManagedObject.h"

//Audio system include so we can actually interact with the system.
#include "CrAudioSystem.h"

//The base type for actual sound files.
#include "CrSound.h"

#include "CrLoadable.h"

//Represents a sound which will be played. This is a component, so it must live inside another object using CrComponent<CrSound>.
//This is paired with CrSound, which is the actual sound file to play.
class CrSoundPlayer : public CrManagedObject
{
	DEF_CLASS(CrSoundPlayer, CrManagedObject);

	friend class CrUI_SoundEditor;

	virtual ~CrSoundPlayer();

	virtual void BinSerialize(CrArchive& Arch) override;

	bool LoadSound();
	void UnloadSound();
	void UpdateSettings();

	bool Play();
	bool Stop();

	float GetDuration();

	virtual void Construct() override;

	CrLoadable<CrSound> LoadableSound;

protected:
	CrAudioSystem* AudioSystem = nullptr;
	CrSoundSetting Settings;
	SoLoud::handle SoundHandle = 0;
};


