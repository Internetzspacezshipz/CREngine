#include "CRE_Texture.hpp"
#include "vk_engine.h"
#include "CRE_Globals.hpp"

void CRE_Texture::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);

	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	if (!bSerializing)
	{
		Handle = Engine->LoadTexture(File.string());
	}
}

Texture* CRE_Texture::GetTextureActual()
{
	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	return Engine->get_texture(Handle);
}

