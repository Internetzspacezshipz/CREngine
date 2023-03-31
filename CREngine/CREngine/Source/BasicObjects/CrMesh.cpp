#include "CrMesh.h"
#include "vk_engine.h"
#include "CrGlobals.h"

REGISTER_CLASS(CrMesh);

CrMesh::~CrMesh()
{
	UnloadMesh();
}

//void CrMesh::Serialize(bool bSerializing, nlohmann::json& TargetJson)
//{
//	Super::Serialize(bSerializing, TargetJson);
//
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);
//
//	VulkanEngine* Engine = CrGlobals::GetEnginePointer();
//	if (!bSerializing)
//	{
//		//empty is such a dumb name for a function that checks if it is empty...
//		if (File.empty() == false)
//		{
//			UploadMesh();
//		}
//	}
//}

void CrMesh::BinSerialize(CrArchive& Arch)
{
	Arch <=> File;

	//If loading
	if (Arch.bSerializing == false)
	{
		UploadMesh();
	}
}

bool CrMesh::UploadMesh()
{
	if (bMeshLoaded)
	{
		return true;
	}

	if (File.empty() == false)
	{
		if (MeshData.load_from_obj(File.generic_string().c_str()))
		{
			VulkanEngine* Engine = CrGlobals::GetEnginePointer();
			Engine->UploadMesh(&MeshData);
			bMeshLoaded = true;
			return true;
		}
	}
	return false;
}

void CrMesh::UnloadMesh()
{
	if (bMeshLoaded == true)
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		Engine->UnloadMesh(&MeshData);
		bMeshLoaded = false;
	}
}
