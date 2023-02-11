#include "CRE_App.hpp"

#include <filesystem>

CRE_App::CRE_App()
{
	auto BasePath = std::filesystem::current_path();
	Window = new CRE_Window(WIDTH, HEIGHT, "New Vulkan Window");

	auto VPath = BasePath / std::filesystem::path("Shaders\\Compiled\\Simple_VertShader.vert.spv");
	auto FPath = BasePath / std::filesystem::path("Shaders\\Compiled\\Simple_FragShader.frag.spv");

	GraphicsPipeline = new CRE_GraphicsPipeline(VPath.string(), FPath.string());
}

CRE_App::~CRE_App()
{
	delete Window;
	delete GraphicsPipeline;
}

void CRE_App::Run()
{
	while (!Window->ShouldClose())
	{
		glfwPollEvents();
	}
}

