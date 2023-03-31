#pragma once

#include "CrManagedObject.hpp"
#include <vk_textures.h>

//In charge of loading textures from disk and uploading to GPU/Vulkan
class CrTexture : public CrManagedObject
{
	DEF_CLASS(CrTexture, CrManagedObject);

	virtual ~CrTexture();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	bool UploadTexture();
	void UnloadTexture();

	//Editor funcs
	virtual void OnRename() override;
	// end editor funcs

	Texture* GetData() { return TextureData.get(); };

	bool ValidData() { return TextureData.get(); }

private:
	UP<Texture> TextureData { nullptr };
};
