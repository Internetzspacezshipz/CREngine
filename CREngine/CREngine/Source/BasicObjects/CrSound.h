#pragma once

#include "CrManagedObject.h"

//Audio system include so we can actually interact with the system.
#include "CrAudioSystem.h"

//Represents a sound which will be played.
class CrSound : public CrManagedObject
{
	DEF_CLASS(CrSound, CrManagedObject);

	friend class CrUI_SoundEditor;
	friend class CrSoundPlayer;

	CrSound();
	virtual ~CrSound();

	virtual void BinSerialize(CrArchive& Arch) override;

	bool Import();

	//Imports the mesh from ImportPath
	bool LoadSound();
	void UnloadSound();

	void PlayThrowaway();
	void StopThrowaway();

	float GetDuration();

	Path ImportPath;

protected:

	CrAudioSystem* AudioSystem = nullptr;
	CrSoundSetting Settings;
	BinArray RawSound;

	SoLoud::Wav WaveObject;
	SoLoud::handle ThrowawayHandle;
};


