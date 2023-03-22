#pragma once

//forward declares to make sure they're not declared as a part of CRE_Globals.
class VulkanEngine;
class CRE_App;
class CRE_KeySystem;

//Holds any global pointers for anything to get.
namespace CRE_Globals
{
	extern inline VulkanEngine*& GetEnginePointer();

	extern inline CRE_App*& GetAppPointer();

	extern inline CRE_KeySystem*& GetKeySystemPointer();
	//TODO: Add settings object pointer here.
}
