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

	if (!bSerializing)
	{
		LoadTexture();
	}
}

bool CRE_Texture::LoadTexture()
{
	if (File.string().size())
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
		Handle = Engine->LoadTexture(File.string());
		return Handle != 0;
	}
	return false;
}

void CRE_Texture::OnRename()
{
	Handle = 0;
}

Texture* CRE_Texture::GetTextureActual()
{
	VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
	return Engine->get_texture(Handle);
}

