#include "CRE_App.hpp"
#include "CRE_RenderSystem.hpp"

//std incl
#include <filesystem>
#include <array>

//glm graphics incl
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


CRE_App::CRE_App()
{
	Window = new CRE_Window(WIDTH, HEIGHT, "New Vulkan Window");

	Device = new CRE_Device(*Window);

	Renderer = new CRE_Renderer(Window, Device);

	LoadGameObjects();
}

CRE_App::~CRE_App()
{
	GameObjects.clear();

	delete Device;
	delete Window;
}

void CRE_App::Run()
{
	CRE_RenderSystem RenderSystem{Device, Renderer->GetSwapChainRenderPass()};

	while (!Window->ShouldClose())
	{
		glfwPollEvents();
		if (auto CommandBuffer = Renderer->BeginFrame())
		{
			Renderer->BeginSwapChainRenderPass(CommandBuffer);
			RenderSystem.RenderGameObjects(CommandBuffer, GameObjects);
			Renderer->EndSwapChainRenderPass(CommandBuffer);
			Renderer->EndFrame();
		}
	}

	//wait until vulkan has cleaned everything up.
	vkDeviceWaitIdle(Device->device());
}

void CRE_App::LoadGameObjects()
{
	std::vector<CRE_Mesh::Vertex> Verticies
	{
		{{0.5f, -0.5f}	,{1.f,0.f,0.f}},
		{{1.0f, 0.5f}	,{0.f,1.f,0.f}},
		{{-0.5f, 0.5f}	,{0.f,0.f,1.f}},
	};

	auto Mesh = std::make_shared<CRE_Mesh>(Device, Verticies);

	CRE_PhysicalGameObject Triangle = CRE_PhysicalGameObject::CreateGameObject();
	Triangle.MeshObject = Mesh;
	Triangle.Transform.Translation.x = .2f;
	Triangle.Transform.Scale = {2.f, .5f};
	Triangle.Transform.Rotation = 0.25f * glm::two_pi<float>();

	GameObjects.push_back(std::move(Triangle));
}
