#pragma once

#include "CrManagedObject.hpp"
#include <vk_mesh.h>

//A mesh which can be loaded into the GPU and used in the scene.
class CrMesh : public CrManagedObject
{
	DEF_CLASS(CrMesh, CrManagedObject);

	virtual ~CrMesh();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	bool UploadMesh();
	void UnloadMesh();

	Mesh* GetData() { return &MeshData; }

private:
	bool bMeshLoaded = false;
	Mesh MeshData;
};


