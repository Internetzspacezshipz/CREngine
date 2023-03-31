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

	TextureData* GetData() { return TexData.get(); };

	bool ValidData() { return TexData.get(); }

private:
	UP<TextureData> TexData { nullptr };
};
