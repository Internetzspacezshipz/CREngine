#include "CrTexture.h"
#include "vk_engine.h"
#include "CrGlobals.h"
#include "CrSerialization.h"

#include <algorithm>

#include <ppl.h>

REGISTER_CLASS_FLAGS(CrTexture, CrClassFlags_Unique);

REGISTER_EXTENSION(CrTexture, ".crim");

CrTexture::CrTexture()
{
	TexData = MkUP<TextureData>();
	CompressionType = BC1;
}

CrTexture::~CrTexture()
{
	//Remember to unload when destroying
	UnloadTexture();

	//Ensure the pixels are deleted.
	DeletePixels();
}

void CrTexture::BinSerialize(CrArchive& Arch)
{
	Arch <=> ImportPath;

	if (Arch.bSerializing)
	{
		//Ensure we do the import if we're serializing.
		Import();
	}

	Arch <=> TextureWidth;
	Arch <=> TextureHeight;
	Arch <=> TextureChannelsActual;
	Arch <=> TextureChannels;
	Arch <=> Image;

	UploadTexture();
}

//todo: thread import
bool CrTexture::Import()
{
	//Clear pixel ptr if we have one already.
	DeletePixels();

	auto FileStr = (BasePath() / ImportPath).string();
	bool bIsFloatingPoint;

	TextureChannels = CrCompression::GetFormatAttributes(CompressionType).Channels;

	CrCompression::GetFileInfo(FileStr, TextureWidth, TextureHeight, TextureChannelsActual, bIsFloatingPoint);
	CrCompression::ImportAndCompress(Image, FileStr, CompressionType);

	RealSizeBytes = Image.size();

	return Image.size();
}

bool CrTexture::UploadTexture()
{
	if (Image.size())
	{
		//If we have a tex already, unload it.
		UnloadTexture();
		//Make a new texture pointer since we might have just deleted it.
		TexData = std::make_unique<TextureData>();

		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		if (vkutil::AllocImage(Engine, Image, TextureWidth, TextureHeight, TextureChannels, GetVkFormat(), GetData()->image))
		{
			Engine->UploadTexture(TexData.get(), GetVkFormat());
			//remove the pixels after our work is all done.
			DeletePixels();
			return true;
		}
	}
	return false;
}

void CrTexture::UnloadTexture()
{
	if (ValidData())
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();
		TextureData* TexPointer = TexData.release();

		//Might not be thread safe because this isn't guaranteed to be deleted after removal from the NextFrameDeletors array...
		Engine->NextFrameDeletors.push_back(
		[TexPointer](VulkanEngine* Engine)
		{
			Engine->UnloadTexture(TexPointer);
			delete TexPointer;
		});
	}
}

void CrTexture::OnRename()
{
	//???? Should we unload and reload here?
}

VkFormat CrTexture::GetVkFormat()
{
	switch (CompressionType)
	{
	case BC1: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	//case BC1: return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
	//case BC1A: return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
	case BC2: return VK_FORMAT_BC2_UNORM_BLOCK;
	case BC3: return VK_FORMAT_BC3_UNORM_BLOCK;
	case BC4U: return VK_FORMAT_BC4_UNORM_BLOCK;
	case BC4S: return VK_FORMAT_BC4_SNORM_BLOCK;
	case BC5U: return VK_FORMAT_BC5_UNORM_BLOCK;
	case BC5S: return VK_FORMAT_BC5_SNORM_BLOCK;
	case BC6HU: return VK_FORMAT_BC6H_UFLOAT_BLOCK;
	case BC6HS: return VK_FORMAT_BC6H_SFLOAT_BLOCK;
	case BC7: return VK_FORMAT_BC7_UNORM_BLOCK;
	default:
		CrLOG("Returned default VkFormat from object {}! Something is very wrong.", GetID().GetString());
		return VK_FORMAT_R8G8B8A8_SRGB;
	}
}

void CrTexture::DeletePixels()
{
	if (Image.size())
	{
		Image.clear();
	}
}

