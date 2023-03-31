#include "CrTexture.h"
#include "vk_engine.h"
#include "CrGlobals.h"
#include "CrSerialization.hpp"

REGISTER_CLASS_FLAGS(CrTexture, CrClassFlags_Unique, CrClassFlags_Transient);

REGISTER_EXTENSION(CrTexture, ".png");

CrTexture::~CrTexture()
{
	//Remember to unload when destroying
	UnloadTexture();
}

//void CrTexture::Serialize(bool bSerializing, nlohmann::json& TargetJson)
//{
//	Super::Serialize(bSerializing, TargetJson);
//
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);
//
//	if (!bSerializing)
//	{
//		UploadTexture();
//	}
//}

void CrTexture::BinSerialize(CrArchive& Arch)
{
	if (!Arch.bSerializing)
	{
		assert(UploadTexture());
	}
}

bool CrTexture::UploadTexture()
{
	if (ValidData())
	{
		return true;
	}

	auto FileStr = GetID().GetString();
	FileStr.append(".png");

	if (FileStr.size())
	{
		VulkanEngine* Engine = CrGlobals::GetEnginePointer();

		TextureData = std::make_unique<Texture>();
		if (vkutil::load_image_from_file(Engine, FileStr.c_str(), TextureData->image))
		{
			Engine->UploadTexture(TextureData.get());
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
		Texture* TexPointer = TextureData.release();

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

