#include "CrApp.h"
#include "CrSerialization.h"
#include "CrVerse.h"

#include "CrGlobals.h"

#include "BasicObjects/CrRenderable.h"

//std incl
#include <filesystem>
#include <array>

//glm graphics incl
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "CrMath.h"
#include "vk_engine.h"

//UI
#include "UserInterface/CrUI_MenuBar.h"
#include <CrTickSystem.h>

CrApp::CrApp()
{

}

CrApp::~CrApp()
{

}

void CrApp::Setup()
{
    CrGlobals::GetEnginePointer()->UIDrawFunction =
    [this]()
    {
        DrawUIObjects();
    };
}

void CrApp::DrawUIObjects()
{
    CrTickSystem::Get().RunTicks();

    for (auto& Element : UIObjects)
    {
        //keep a copy of the temp shared pointer here in case the UI tries to remove itself before it's done execution.
        SP<CrUI_Base> TempSP = Element.second;
        TempSP->DrawUI();
    }

    //Remove all null items that might have been deleted at any time before..
    RemoveByPredicate(UIObjects, [](const Pair<const CrID, SP<CrUI_Base>>& Item)->bool { return Item.second.get() == nullptr; });
}

void CrApp::LoadInitialGameFiles()
{
    //Add default menu bar
    MakeUI(CrUI_MenuBar::StaticClass());
}

void CrApp::Cleanup()
{
    //TODO: Add save file somewhere.
}

SP<CrUI_Base> CrApp::MakeUI(CrID Class)
{
    SP<CrUI_Base> NewlyMade = MakeUINoAdd(Class);
    if (NewlyMade != nullptr)
    {
        AddUI(NewlyMade->GetID(), NewlyMade);
    }
    return NewlyMade;
}

SP<CrUI_Base> CrApp::MakeUINoAdd(CrID Class)
{
    if (Class.IsValidID())
    {
        CrObjectFactory& ObjectFactory = CrObjectFactory::Get();
        if (SP<CrUI_Base> NewUI = DCast<CrUI_Base>(ObjectFactory.Create(Class)))
        {
            return NewUI;
        }
    }
    return SP<CrUI_Base>(nullptr);
}

void CrApp::RemoveUI(CrID Name)
{
    auto Found = UIObjects.find(Name);
    if (Found != UIObjects.end())
    {
        //Remove by reseting - we want to keep the entry in the array until we're done looping the array.
        Found->second.reset();
    }
}

void CrApp::RemoveUI(CrUI_Base* ActualUI)
{
    RemoveUI(ActualUI->GetID());
}

void CrApp::LoadGameObjects()
{
    CrObjectFactory& ObjectFactory = CrObjectFactory::Get();
    CrClassConcrete<CrRenderable>& RenderableClass = CrClassConcrete<CrRenderable>::Get();

    //make test 2d box.
    if (true)
    {
        CrTransformBox2D Box;
        Box.TopLeft = { -1.f, -1.f };
        Box.BotRight = { 0.f, 0.f };
        Box.CameraDistance = 0.1f;

        //Box.Scale(-1.f, -1.f);

        std::vector<Vertex> Tris = Box.GetRenderTris();

        //std::shared_ptr<CrMesh> BoxPtr = std::make_unique<CrMesh>(Device, Tris);

        //Instantiate new renderable object from class.
        auto BoxGameObject = RenderableClass.Create();

        //BoxGameObject->MeshObject = BoxPtr;

        //GameObjects.push_back(BoxGameObject);
    }

    //Make test 2d box 2
    if (true)
    {
        CrTransformBox2D Box;
        Box.TopLeft = { 0.f, 0.f };
        Box.BotRight = { 1.f, 1.f } ;
        Box.CameraDistance = 0.1f;

        //Box.Scale(-1.f, -1.f);

        std::vector<Vertex> Tris = Box.GetRenderTris();

       // std::shared_ptr<CrMesh> BoxPtr = std::make_unique<CrMesh>(Device, Tris);

        //Instantiate new renderable object from class.
        auto BoxGameObject = RenderableClass.Create();

        //BoxGameObject->MeshObject = BoxPtr;

        //GameObjects.push_back(BoxGameObject);
    }
}

void CrApp::AddUI(CrID Name, SP<CrUI_Base> NewUI)
{
    if (!UIObjects.contains(Name))
    {
        UIObjects.emplace(Name, NewUI);
    }
}