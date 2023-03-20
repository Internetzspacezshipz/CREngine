#include "CRE_App.hpp"
#include "CRE_Serialization.hpp"
#include "CRE_AssetList.h"
#include "CRE_Globals.hpp"


#include "BasicObjects/CRE_2DRenderable.hpp"

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

}

CRE_App::~CRE_App()
{

}

void CRE_App::SetupEnginePointer(VulkanEngine* InEnginePointer)
{
    CRE_Globals::GetEnginePointer() = InEnginePointer;
}

void CRE_App::LoadInitialGameFiles()
{
    //Load main files;
    CRE_Serialization& Serializer = CRE_Serialization::Get();


    //Load the root object and initialize a new asset list object with it to load all other relevant data.
    nlohmann::json Manifest = Serializer.LoadManifest();
    if (Manifest.is_null())
    {
        //Load default/hardcoded objects. objects.
        LoadGameObjects();
    }
    RootObject = CRE_ObjectFactory::Get().Create<CRE_AssetList>();
    RootObject->Serialize(false, Manifest);
}

void CRE_App::SaveGame()
{
    CRE_Serialization& Serializer = CRE_Serialization::Get();

    //Save root object. Maybe we can make a save game object later on as well as other types of similar uses (settings object, etc).
    nlohmann::json NewManifest;
    RootObject->Serialize(false, NewManifest);
    delete RootObject;
    Serializer.SaveManifest(NewManifest);
}

void CRE_App::LoadGameObjects()
{
    CRE_ObjectFactory& ObjectFactory = CRE_ObjectFactory::Get();
    CRE_Class<CRE_2DRenderable>& RenderableClass = CRE_Class<CRE_2DRenderable>::Get();

    //make test 2d box.
    if (true)
    {
        CRE_TransformBox2D Box;
        Box.TopLeft = { -1.f, -1.f };
        Box.BotRight = { 0.f, 0.f };
        Box.CameraDistance = 0.1f;

        //Box.Scale(-1.f, -1.f);

        std::vector<CRE_Vertex> Tris = Box.GetRenderTris();

        //std::shared_ptr<CRE_Mesh> BoxPtr = std::make_unique<CRE_Mesh>(Device, Tris);

        //Instantiate new renderable object from class.
        auto BoxGameObject = RenderableClass.Create();

        //BoxGameObject->MeshObject = BoxPtr;

        //GameObjects.push_back(BoxGameObject);
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

       // std::shared_ptr<CRE_Mesh> BoxPtr = std::make_unique<CRE_Mesh>(Device, Tris);

        //Instantiate new renderable object from class.
        auto BoxGameObject = RenderableClass.Create();

        //BoxGameObject->MeshObject = BoxPtr;

        //GameObjects.push_back(BoxGameObject);
    }
}
