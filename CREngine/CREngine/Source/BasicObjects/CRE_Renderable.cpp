#include "CRE_Renderable.h"
#include "CRE_Globals.h"
#include "vk_Engine.h"

#include "CRE_Material.h"
#include "CRE_Mesh.h"

REGISTER_CLASS(CRE_Renderable);

CRE_Renderable::~CRE_Renderable()
{
	UnloadRenderable();
}

void CRE_Renderable::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	//advise to do this for each child class of this one.
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, MaterialOb);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, MeshOb);

	if (!bSerializing)
	{
		LoadRenderable();
	}
}

void CRE_Renderable::SetRenderingEnabled(bool bRenderingEnabled)
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

void CRE_Renderable::LoadRenderable()
{
	if (MeshOb.IsLoadedOrLoadable())
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
		MeshOb.SafeLoad();

		//Ensure mesh is uploaded.
		MeshOb->UploadMesh();
		bHasBeenLoaded = true;

		//Make a shared pointer from ourself.
		SP<RenderObject> FromThis(static_cast<SP<RenderObject>>(shared_from_this()));
		Engine->SubmitRenderable(FromThis);
	}
}

void CRE_Renderable::UnloadRenderable()
{
	if (bHasBeenLoaded)
	{
		//Do we even need to do anything to unload? This is a shared pointer.
	}
}

Mesh* CRE_Renderable::GetMesh()
{
	return MeshOb->GetData();
}

Material* CRE_Renderable::GetMaterial()
{
	//Later turn this into a thing that can return multiple materials.
	return MaterialOb->GetData();
}
