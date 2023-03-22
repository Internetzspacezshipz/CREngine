#include "CRE_Mesh.hpp"
#include "vk_engine.h"
#include "CRE_Globals.hpp"

REGISTER_CLASS(CRE_Mesh, CRE_ManagedObject);

void CRE_Mesh::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);

	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	if (!bSerializing)
	{
		Handle = Engine->LoadMesh(File.string());
	}
}

Mesh* CRE_Mesh::GetMeshActual()
{
	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	return Engine->get_mesh(Handle);
}
