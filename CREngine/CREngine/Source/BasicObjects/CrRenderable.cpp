#include "CrRenderable.h"
#include "CrGlobals.h"
#include "vk_Engine.h"

#include "CrMaterial.h"
#include "CrMesh.h"

REGISTER_CLASS(CrRenderable);

CrRenderable::~CrRenderable()
{
	UnloadRenderable();
}

//void CrRenderable::Serialize(bool bSerializing, nlohmann::json& TargetJson)
//{
//	//advise to do this for each child class of this one.
//	Super::Serialize(bSerializing, TargetJson);
//
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, MaterialOb);
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, MeshOb);
//
//	if (!bSerializing)
//	{
//		LoadRenderable();
//	}
//}

void CrRenderable::BinSerialize(CrArchive& Arch)
{
	Arch <=> MaterialOb;
	Arch <=> MeshOb;

	if (!Arch.bSerializing)
	{
		LoadRenderable();
	}
}

void CrRenderable::SetRenderingEnabled(bool bRenderingEnabled)
{
	if (bRenderingEnabled)
	{
		bRenderEnable = true;
	}
	else
	{
		bRenderEnable = false;
	}
}

void CrRenderable::LoadRenderable()
{
	if (MeshOb.IsLoadedOrLoadable())
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		MeshOb.SafeLoad();

		//Ensure mesh is uploaded.
		MeshOb->UploadMesh();
		bHasBeenLoaded = true;

		//Make a shared pointer from ourself.
		SP<RenderObject> FromThis(static_cast<SP<RenderObject>>(shared_from_this()));
		Engine->SubmitRenderable(FromThis);
	}
}

void CrRenderable::UnloadRenderable()
{
	if (bHasBeenLoaded)
	{
		//Do we even need to do anything to unload? This is a shared pointer.
	}
}

Mesh* CrRenderable::GetMesh()
{
	return MeshOb->GetData();
}

MaterialData* CrRenderable::GetMaterial()
{
	//Later turn this into a thing that can return multiple materials.
	return MaterialOb->GetData();
}
