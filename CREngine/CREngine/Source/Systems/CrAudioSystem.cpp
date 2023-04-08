#include "CrAudioSystem.h"
#include "CrPaths.h"

#include "soloud_file.h"

//#include <SDL.h>

#define CHANNELS    2
#define SAMPLE_RATE 48000

template<typename Type>
Type StoMS(const Type& Value)
{
    return Value * (Type)1000;
}

bool CrAudioSystem::InitializeAudioEngine()
{
    bool bSuccess = AudioEngine.init() == 0;
    if (!bSuccess)
    {
        return false;
    }


    TestWav.load("C:\\GitHub\\CREngine\\CREngine\\CREngine\\Assets\\TestAudio1.ogg");
    AudioEngine.play(TestWav);
    return bSuccess;
}

bool CrAudioSystem::ImportAudio(BinArray& DataOut, const Path& TargetPath)
{
    /*
    SoLoud::Wav Wave;

    //Use SoLoud's file loading in order to follow the exact same path that SoLoud would normally.
    SoLoud::MemoryFile dr;


    const unsigned char* aData;
    unsigned int aDataLength; 
    bool aCopy; 
    bool aTakeOwnership;
    dr.openMem(aMem, aLength, aCopy, aTakeOwnership);

    auto Result = Wave.load(TargetPath.generic_string().c_str());
    Wave.loadMem;



    return Result == 0;*/
    return true;
}

bool CrAudioSystem::PlayAudio(SoLoud::Wav& OutData)
{
    auto* Item = OutData.createInstance();
    return Item != nullptr;
}

bool CrAudioSystem::StopAudio()
{
    return false;
}

void CrAudioSystem::DestroySound()
{
}

void CrAudioSystem::SetSoundSettings(const CrSoundSetting& InSettings)
{
}

void CrAudioSystem::DestroyAudioEngine()
{
    AudioEngine.deinit();
}
