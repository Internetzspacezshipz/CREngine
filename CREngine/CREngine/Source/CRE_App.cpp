#include "CRE_App.hpp"
#include "CRE_RenderSystem.hpp"
#include "CRE_Serialization.hpp"
#include "CRE_RenderableObject.hpp"

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

    //Load main files;
    CRE_Serialization& Serializer = CRE_Serialization::Get();
    CRE_ObjectFactory& ObjectFactory = CRE_ObjectFactory::Get();

    //Load objects
    {
        LoadGameObjects();

        nlohmann::json Manifest = Serializer.LoadManifest();

        for (auto& Elem : Manifest)
        {
            CRE_ManagedObject* NewObject = ObjectFactory.Create(Elem[0]);
            GameObjects.push_back(NewObject);
            NewObject->Serialize(false, Elem[1]);
        }
    }

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

    //Save objects.
    {
        nlohmann::json NewManifest;
        for (auto* Elem : GameObjects)
        {
            nlohmann::json Inner;
            Elem->Serialize(true, Inner);
            nlohmann::json BaseJson{ Elem->GetClass(), Inner };
            NewManifest.push_back(BaseJson);
        }

        Serializer.SaveManifest(NewManifest);
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
    CRE_ObjectFactory& ObjectFactory = CRE_ObjectFactory::Get();

    //3d box
    if (false)
    {
        std::shared_ptr<CRE_Mesh> Cube = createCubeModel(Device, glm::vec3{ 0.f, 0.f, 0.f });

        //Instantiate new renderable object from class.
        auto CubeGameObject = ObjectFactory.Create(CRE_RenderableObject::StaticClass());

        CubeGameObject->MeshObject = Cube;
        CubeGameObject->Transform.Translation = { 0.f, 0.f, 0.5f };
        CubeGameObject->Transform.Scale = { 0.5f, 0.5f, 0.5f };

        GameObjects.push_back(CubeGameObject);
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

        //Instantiate new renderable object from class.
        auto BoxGameObject = ObjectFactory.Create(CRE_RenderableObject::StaticClass());

        BoxGameObject->MeshObject = BoxPtr;

        GameObjects.push_back(BoxGameObject);
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

        //Instantiate new renderable object from class.
        auto BoxGameObject = ObjectFactory.Create(CRE_RenderableObject::StaticClass());

        BoxGameObject->MeshObject = BoxPtr;

        GameObjects.push_back(BoxGameObject);
    }
}
