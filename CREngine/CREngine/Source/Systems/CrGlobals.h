#pragma once

//forward declares to make sure they're not declared as a part of CrGlobals.
class VulkanEngine;
class CrApp;
class CrKeySystem;
class CrAudioSystem;

//Holds any global pointers for anything to get.
namespace CrGlobals
{
	extern inline VulkanEngine*& GetEnginePointer();

	extern inline CrApp*& GetAppPointer();

	extern inline CrKeySystem*& GetKeySystemPointer();

	extern inline CrAudioSystem*& GetAudioSystemPointer();
	//TODO: Add settings object pointer here.
}
