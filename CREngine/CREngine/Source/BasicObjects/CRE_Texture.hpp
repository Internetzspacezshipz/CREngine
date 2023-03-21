#pragma once

#include "CRE_ManagedObject.hpp"
#include <vk_textures.h>

//In charge of loading textures from disk and managing them.
class CRE_Texture : public CRE_ManagedObject
{
	DEF_CLASS(CRE_Texture, CRE_ManagedObject);

	virtual ~CRE_Texture()
	{}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	Texture* GetTextureActual();

	std::filesystem::path File;
	AssetHandle Handle;
};

REGISTER_CLASS(CRE_Texture, CRE_ManagedObject);
