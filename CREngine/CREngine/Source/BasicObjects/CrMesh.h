#pragma once

#include "CrManagedObject.hpp"
#include <vk_mesh.h>

//A mesh which can be loaded into the GPU and used in the scene.
class CrMesh : public CrManagedObject
{
	DEF_CLASS(CrMesh, CrManagedObject);

	virtual ~CrMesh();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) ovMeserride;
	virtual void BinSerialize(CrArchive& Arch) override;

	//Imports the mesh from ImportPath
	bool Import();

	bool UploadMesh();
	void UnloadMesh();

	MeshData* GetData() { return &Data; }

	Path ImportPath;

private:
	bool bMeshLoaded = false;
	MeshData Data;
};


