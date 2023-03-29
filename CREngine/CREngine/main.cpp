
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC_NEW

#include <crtdbg.h>
#include <assert.h>
#endif


#include "CRE_App.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <vk_engine.h>

int main(int argc, char* argv[])
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	VulkanEngine engine;
	CRE_App MainApp;
	engine.init();

	MainApp.SetupGlobalVariables(&engine);
	MainApp.LoadInitialGameFiles();

	engine.run();

	MainApp.SaveGame();

	return 0;
}
