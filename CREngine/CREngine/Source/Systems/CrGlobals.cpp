#include "CrGlobals.h"
//forward declares to make sure they're not declared as a part of CrGlobals.

VulkanEngine*& CrGlobals::GetEnginePointer()
{
	static VulkanEngine* Engine = nullptr;
	return Engine;
}

CrApp*& CrGlobals::GetAppPointer()
{
	static CrApp* App = nullptr;
	return App;
}

CrKeySystem*& CrGlobals::GetKeySystemPointer()
{
	static CrKeySystem* Key = nullptr;
	return Key;
}

CrAudioSystem*& CrGlobals::GetAudioSystemPointer()
{
	static CrAudioSystem* Audio = nullptr;
	return Audio;
}

CrNetwork*& CrGlobals::GetNetworkPointer()
{
	static CrNetwork* Network = nullptr;
	return Network;
}
