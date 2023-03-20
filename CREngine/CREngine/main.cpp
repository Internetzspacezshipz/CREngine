#include "CRE_App.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

/*int main()
{
	CRE_App Application{};

	try
	{
		Application.Run();
	}
	catch (const std::exception& Exception)
	{
		std::cerr << Exception.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}*/

#include <vk_engine.h>

int main(int argc, char* argv[])
{
	VulkanEngine engine;
	CRE_App MainApp;

	engine.init();

	engine.run();

	MainApp.LoadInitialGameFiles();

	MainApp.SetupEnginePointer(&engine);

	engine.cleanup();

	MainApp.SaveGame();

	return 0;
}
