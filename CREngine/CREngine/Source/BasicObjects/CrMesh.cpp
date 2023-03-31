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

	auto FileStr = MakeAssetReference().GetString();

	if (FileStr.empty() == false)
	{
		if (MeshData.load_from_obj(FileStr.c_str()))
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
