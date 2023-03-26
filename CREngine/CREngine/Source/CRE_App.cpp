#include "CRE_App.hpp"
#include "CRE_Serialization.hpp"
#include "CRE_AssetList.hpp"
#include "CRE_Globals.hpp"


#include "BasicObjects/CRE_Renderable.hpp"

//std incl
#include <filesystem>
#include <array>

//glm graphics incl
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "CRE_Math.hpp"
#include "UserInterface/CRE_UI_AssetListEditor.hpp"
#include "vk_engine.h"

CRE_App::CRE_App()
{

}

CRE_App::~CRE_App()
{

}

void CRE_App::SetupGlobalVariables(VulkanEngine* InEnginePointer)
{
    CRE_Globals::GetEnginePointer() = InEnginePointer;
    CRE_Globals::GetKeySystemPointer() = &InEnginePointer->_KeySystem;
    CRE_Globals::GetAppPointer() = this;
    
    InEnginePointer->UIDrawFunction =
    [this]()
    {
        DrawUIObjects();
    };
}

void CRE_App::DrawUIObjects()
{
    for (SP<CRE_UI_Base> Element : UIObjects)
    {
        Element->DrawUI();
    }
}

void CRE_App::LoadInitialGameFiles()
{
    //Load main files;
    CRE_Serialization& Serializer = CRE_Serialization::Get();

    //Load the root object and initialize a new asset list object with it to load all other relevant data.
    RootObject = Serializer.LoadManifest();

   // SP<CRE_UI_AssetListEditor> BaseAssetListEditor = CRE_ObjectFactory::Get().Create<CRE_UI_AssetListEditor>();

    auto id = CRE_UI_AssetListEditor::StaticClass();
    SP<CRE_Obj> BaseAssetListEditor = CRE_ObjectFactory::Get().Create(id);

    auto casted = DCast<CRE_UI_AssetListEditor>(BaseAssetListEditor);

    UIObjects.push_back(casted);
}

void CRE_App::SaveGame()
{
    //for now don't save anything - it will be controlled through the UI.
    CRE_Serialization& Serializer = CRE_Serialization::Get();

    //Save root object. Maybe we can make a save game object later on as well as other types of similar uses (settings object, etc).
    Serializer.Save(RootObject);

    //Make sure to delete the root object. - maybe later we can wrap this in an SPtr
    RootObject.reset();
}

void CRE_App::LoadGameObjects()
{
    CRE_ObjectFactory& ObjectFactory = CRE_ObjectFactory::Get();
    CRE_Class<CRE_Renderable>& RenderableClass = CRE_Class<CRE_Renderable>::Get();

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
