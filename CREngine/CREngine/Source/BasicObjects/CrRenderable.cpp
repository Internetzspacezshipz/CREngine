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
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Material);
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Mesh);
//
//	if (!bSerializing)
//	{
//		LoadRenderable();
//	}
//}

void CrRenderable::BinSerialize(CrArchive& Arch)
{
	Arch <=> Material;
	Arch <=> Mesh;

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
	if (Mesh.IsLoadedOrLoadable())
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		Mesh.SafeLoad();

		//Ensure mesh is uploaded.
		Mesh->UploadMesh();
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

MeshData* CrRenderable::GetMesh()
{
	return Mesh->GetData();
}

MaterialData* CrRenderable::GetMaterial()
{
	//Later turn this into a thing that can return multiple materials.
	return Material->GetData();
}
