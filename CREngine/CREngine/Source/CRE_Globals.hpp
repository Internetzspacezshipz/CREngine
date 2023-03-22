#pragma once

//forward declares to make sure they're not declared as a part of CRE_Globals.
class VulkanEngine;
class CRE_App;

//Holds any global pointers for anything to get.
namespace CRE_Globals
{
	extern inline VulkanEngine*& GetEnginePointer();

	extern inline CRE_App*& GetAppPointer();
	//TODO: Add settings object pointer here.
}
