
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

#include <crtdbg.h>
#include <assert.h>
#endif

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include "CrApp.h"
#include <vk_engine.h>
#include "CrAudioSystem.h"
#include "CrGlobals.h"

#include "CrNetwork.h"

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//Setup main systems and set up their global pointers.

	VulkanEngine RenderEngine;
	CrGlobals::GetEnginePointer() = &RenderEngine;

	CrApp MainApp;
	CrGlobals::GetAppPointer() = &MainApp;

	CrAudioSystem AudioSystem;
	CrGlobals::GetAudioSystemPointer() = &AudioSystem;

	CrKeySystem InputSystem;
	CrGlobals::GetKeySystemPointer() = &InputSystem;

	CrNetwork Network;
	CrGlobals::GetNetworkPointer() = &Network;

	RenderEngine.init();

	MainApp.Setup();

	assert(AudioSystem.InitializeAudioEngine());

	MainApp.LoadInitialGameFiles();


	//main render loop here.
	RenderEngine.run();


	MainApp.Cleanup();

	AudioSystem.DestroyAudioEngine();

	return 0;
}
