#pragma once

#include "CRE_ManagedObject.hpp"
#include <vk_textures.h>

//In charge of loading textures from disk and uploading to GPU/Vulkan
class CRE_Texture : public CRE_ManagedObject
{
	DEF_CLASS(CRE_Texture, CRE_ManagedObject);

	virtual ~CRE_Texture();

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	bool UploadTexture();
	void UnloadTexture();

	//Editor funcs
	virtual void OnRename() override;
	// end editor funcs

	Texture* GetData() { return TextureData.get(); };

	bool ValidData() { return TextureData.get(); }

	Path File;

private:
	UP<Texture> TextureData { nullptr };
};

