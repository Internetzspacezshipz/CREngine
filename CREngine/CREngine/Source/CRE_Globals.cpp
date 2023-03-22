#include "CRE_Globals.hpp"
//forward declares to make sure they're not declared as a part of CRE_Globals.

VulkanEngine*& CRE_Globals::GetEnginePointer()
{
	static VulkanEngine* Engine;
	return Engine;
}

CRE_App*& CRE_Globals::GetAppPointer()
{
	static CRE_App* App;
	return App;
}
