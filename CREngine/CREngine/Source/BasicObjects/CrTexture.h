#pragma once

#include "CrManagedObject.h"
#include "Utilities/CrCompression.h"
#include <vk_textures.h>

//In charge of loading textures from disk and uploading to GPU/Vulkan
//TODO: change the format to tga.
class CrTexture : public CrManagedObject
{
	DEF_CLASS(CrTexture, CrManagedObject);

	virtual ~CrTexture();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;
	virtual void BinSerialize(CrArchive& Arch) override;

	bool Import();

	bool UploadTexture();
	void UnloadTexture();

	//Editor funcs
	virtual void OnRename() override;
	// end editor funcs

	TextureData* GetData() { return TexData.get(); };

	bool ValidData() { return TexData.get(); }

	virtual void Construct() override;

	VkFormat GetVkFormat();

	Path ImportPath;

	CrTextureFormatTypes CompressionType;

	int TextureWidth = 0;
	int TextureHeight = 0;
	int TextureChannelsActual = 0;
	int TextureChannels = 0;
	size_t RealSizeBytes = 0;

	size_t GetSizeBytes() const
	{
		return (size_t)TextureWidth * (size_t)TextureHeight * (size_t)TextureChannels;
	}
	

	size_t GetSizeBytesReal() const
	{
		return RealSizeBytes;//(size_t)TextureWidth * (size_t)TextureHeight * (size_t)TextureChannels;
	}

private:
	void DeletePixels();

	BinArray Image;

	UP<TextureData> TexData { nullptr };
};
