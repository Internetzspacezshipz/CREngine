#pragma once

#include "CrManagedObject.h"
#include <vk_mesh.h>

//A mesh which can be loaded into the GPU and used in the scene.
class CrMesh : public CrManagedObject
{
	DEF_CLASS(CrMesh, CrManagedObject);

	virtual ~CrMesh();

	virtual void BinSerialize(CrArchive& Arch) override;

	//Imports the mesh from ImportPath
	bool Import();

	bool UploadMesh();
	void UnloadMesh();

	MeshData* GetData() { return &Data; }

	Path ImportPath;

	void MakeDefault();
private:
	bool bMeshLoaded = false;
	MeshData Data;
};


