#include "CrSound.h"
#include "CrSerialization.h"
#include "CrGlobals.h"

//for import of file.
#include "soloud_file.h"

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
	//Clear so we don't have any junk data.
	UnloadSound();
	auto FileStr = (BasePath() / ImportPath).string();

	//Use SoLoud's file loading in order to follow the exact same path that SoLoud would normally.
	SoLoud::DiskFile df;

	df.open(FileStr.c_str());
	RawSound.resize(df.length());
	df.read((unsigned char*)RawSound.data(), df.length());

	return RawSound.size();
}

bool CrSound::LoadSound()
{
	if (RawSound.size())
	{
		WaveObject.loadMem((unsigned char*)RawSound.data(), RawSound.size(), false, false);
	}
	return RawSound.size();
}

void CrSound::UnloadSound()
{
	RawSound.clear();
}

void CrSound::PlayThrowaway()
{
	if (RawSound.size())
	{
		AudioSystem->GetEngine()->play(WaveObject);
	}
}

float CrSound::GetDuration()
{
	return WaveObject.getLength();
}

void CrSound::Construct()
{
	AudioSystem = CrGlobals::GetAudioSystemPointer();
}
