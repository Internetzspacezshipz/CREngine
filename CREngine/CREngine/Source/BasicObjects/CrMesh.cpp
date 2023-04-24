#include "CrMesh.h"
#include "vk_engine.h"
#include "CrGlobals.h"
#include "CrSerialization.h"

REGISTER_CLASS_FLAGS(CrMesh, CrClassFlags_Unique);

REGISTER_EXTENSION(CrMesh, ".crob");

CrMesh::CrMesh()
{
	Data = MkUP<MeshData>();
}

CrMesh::~CrMesh()
{
	UnloadMesh();
}

void CrMesh::BinSerialize(CrArchive& Arch)
{
	Arch <=> ImportPath;

	//Import mesh.
	if (Arch.bSerializing)
	{
		Import();
	}

	Arch <=> Data->Verts;

	//If loading
	if (Arch.bSerializing == false)
	{
		UploadMesh();
	}
}

bool CrMesh::Import()
{
	bool bImportSuccess = false;

	if (!ImportPath.empty())
	{
		Data->Verts = {};
		bImportSuccess = Data->LoadFromObj(ImportPath);
	}

	if (!bImportSuccess)
	{
		MakeDefault();
		return false;
	}

	return true;
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
		if (Data->LoadFromObj(FileStr.c_str()))
		{
			VulkanEngine* Engine = CrGlobals::GetEnginePointer();
			Engine->UploadMesh(Data.get());
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

		//Ensure we use a next frame deletor for the mesh, since it might still be in use when this object is destroyed.
		MeshData* MeshDataPtr = Data.release();
		Engine->NextFrameDeletors.push_back(
		[MeshDataPtr](VulkanEngine* Engine)
		{
			Engine->UnloadMesh(MeshDataPtr);
			delete MeshDataPtr;
		});

		bMeshLoaded = false;
	}
}

void CrMesh::MakeDefault()
{
	//setup standard mesh.
	Data->MakeFromShape(ShapeQuad);
}
