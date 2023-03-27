#include "CRE_Texture.hpp"
#include "vk_engine.h"
#include "CRE_Globals.h"

REGISTER_CLASS_FLAGS(CRE_Texture, CRE_ClassFlags_Unique);

CRE_Texture::~CRE_Texture()
{

}

void CRE_Texture::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);

	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	if (!bSerializing)
	{
		if (File.string().size())
		{
			Handle = Engine->LoadTexture(File.string());
		}
	}
}

Texture* CRE_Texture::GetTextureActual()
{
	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	return Engine->get_texture(Handle);
}

