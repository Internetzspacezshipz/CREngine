#include "CRE_Mesh.h"
#include "vk_engine.h"
#include "CRE_Globals.h"

REGISTER_CLASS(CRE_Mesh);

CRE_Mesh::~CRE_Mesh()
{
	UnloadMesh();
}

void CRE_Mesh::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);

	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	if (!bSerializing)
	{
		//empty is such a dumb name for a function that checks if it is empty...
		if (File.empty() == false)
		{
			UploadMesh();
		}
	}
}

bool CRE_Mesh::UploadMesh()
{
	if (bMeshLoaded)
	{
		return true;
	}

	if (File.empty() == false)
	{
		if (MeshData.load_from_obj(File.generic_string().c_str()))
		{
			VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
			Engine->UploadMesh(&MeshData);
			bMeshLoaded = true;
			return true;
		}
	}
	return false;
}

void CRE_Mesh::UnloadMesh()
{
	if (bMeshLoaded == true)
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
		Engine->UnloadMesh(&MeshData);
		bMeshLoaded = false;
	}
}
