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

#include "CRE_Math.hpp"

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

std::unique_ptr<CRE_Mesh> createCubeModel(CRE_Device* device, glm::vec3 offset)
{
    std::vector<CRE_Vertex> vertices
    {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
    };

    for (auto& v : vertices)
    {
        v.Position += offset;
    }
    return std::make_unique<CRE_Mesh>(device, vertices);
}

void CRE_App::LoadGameObjects()
{
    //3d box
    if (false)
    {
        std::shared_ptr<CRE_Mesh> Cube = createCubeModel(Device, glm::vec3{ 0.f, 0.f, 0.f });

        auto CubeGameObject = CRE_PhysicalGameObject::CreateGameObject();
        CubeGameObject.MeshObject = Cube;

        CubeGameObject.Transform.Translation = { 0.f, 0.f, 0.5f };
        CubeGameObject.Transform.Scale = { 0.5f, 0.5f, 0.5f };

        GameObjects.push_back(std::move(CubeGameObject));
    }

    //make test 2d box.
    if (true)
    {
        CRE_TransformBox2D Box;
        Box.TopLeft = { -1.f, -1.f };
        Box.BotRight = { 0.f, 0.f };
        Box.CameraDistance = 0.1f;

        //Box.Scale(-1.f, -1.f);

        std::vector<CRE_Vertex> Tris = Box.GetRenderTris();

        std::shared_ptr<CRE_Mesh> BoxPtr = std::make_unique<CRE_Mesh>(Device, Tris);

        auto BoxGameObject = CRE_PhysicalGameObject::CreateGameObject();

        BoxGameObject.MeshObject = BoxPtr;

        GameObjects.push_back(std::move(BoxGameObject));
    }

    //Make test 2d box 2
    if (true)
    {
        CRE_TransformBox2D Box;
        Box.TopLeft = { 0.f, 0.f };
        Box.BotRight = { 1.f, 1.f } ;
        Box.CameraDistance = 0.1f;

        //Box.Scale(-1.f, -1.f);

        std::vector<CRE_Vertex> Tris = Box.GetRenderTris();

        std::shared_ptr<CRE_Mesh> BoxPtr = std::make_unique<CRE_Mesh>(Device, Tris);

        auto BoxGameObject = CRE_PhysicalGameObject::CreateGameObject();

        BoxGameObject.MeshObject = BoxPtr;

        GameObjects.push_back(std::move(BoxGameObject));
    }
}
