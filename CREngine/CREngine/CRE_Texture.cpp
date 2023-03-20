#include "CRE_Texture.hpp"
#include "vk_engine.h"
#include "CRE_Globals.hpp"

void CRE_Texture::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);


	CRE_Globals::GetEnginePointer()->(MeshActual);
}
