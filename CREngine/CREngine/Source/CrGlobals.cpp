#include "CrGlobals.h"
//forward declares to make sure they're not declared as a part of CrGlobals.

VulkanEngine*& CrGlobals::GetEnginePointer()
{
	static VulkanEngine* Engine;
	return Engine;
}

CrApp*& CrGlobals::GetAppPointer()
{
	static CrApp* App;
	return App;
}

CrKeySystem*& CrGlobals::GetKeySystemPointer()
{
	static CrKeySystem* App;
	return App;
}
