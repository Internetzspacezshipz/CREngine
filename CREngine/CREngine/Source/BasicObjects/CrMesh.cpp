#include "CrMesh.h"
#include "vk_engine.h"
#include "CrGlobals.h"
#include "CrSerialization.hpp"

REGISTER_CLASS_FLAGS(CrMesh, CrClassFlags_Unique, CrClassFlags_Transient, CrClassFlags_DataOnly);

REGISTER_EXTENSION(CrMesh, ".obj");

CrMesh::~CrMesh()
{
	UnloadMesh();
}

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

	String FileStr = MakeAssetReference().GetString();

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
