#pragma once

#include "CrManagedObject.h"

//Audio system include so we can actually interact with the system.
#include "CrAudioSystem.h"

//Represents a sound which will be played. This is a component, so it must live inside another object using CrComponent<CrSound>.
//This is paired with an actual audio file which this asset will play
class CrSound : public CrManagedObject
{
	DEF_CLASS(CrSound, CrManagedObject);

	friend class CrUI_SoundEditor;

	virtual ~CrSound();

	virtual void BinSerialize(CrArchive& Arch) override;

	bool Import();

	//Imports the mesh from ImportPath
	bool LoadSound();
	void UnloadSound();
	void UpdateSettings();

	bool Play();
	bool Stop();

	float GetDuration();
	virtual void Construct() override;

	Path ImportPath;

protected:

	CrAudioSystem* AudioSystem = nullptr;
	bool bInitialized = false;
	CrSoundSetting Settings;
	BinArray RawSound;
};


