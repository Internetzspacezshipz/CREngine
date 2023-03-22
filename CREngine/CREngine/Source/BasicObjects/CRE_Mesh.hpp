#pragma once

#include "CRE_ManagedObject.hpp"
#include <vk_mesh.h>

//A mesh which can be loaded into the GPU and used in the scene.
class CRE_Mesh : public CRE_ManagedObject
{
	DEF_CLASS(CRE_Mesh, CRE_ManagedObject);

	virtual ~CRE_Mesh()
	{}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	Mesh* GetMeshActual();

	std::filesystem::path File;
	AssetHandle Handle;
};


