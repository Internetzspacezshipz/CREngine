#include "CRE_Texture.h"
#include "vk_engine.h"
#include "CRE_Globals.h"

REGISTER_CLASS_FLAGS(CRE_Texture, CRE_ClassFlags_Unique);

CRE_Texture::~CRE_Texture()
{
	//Remember to unload when destroying
	UnloadTexture();
}

void CRE_Texture::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, File);

	if (!bSerializing)
	{
		UploadTexture();
	}
}

bool CRE_Texture::UploadTexture()
{
	if (ValidData())
	{
		return true;
	}

	if (File.string().size())
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();

		TextureData = std::make_unique<Texture>();
		if (vkutil::load_image_from_file(Engine, File.generic_string().c_str(), TextureData->image))
		{
			Engine->UploadTexture(TextureData.get());
			return true;
		}
	}
	return false;
}

void CRE_Texture::UnloadTexture()
{
	if (ValidData())
	{
		VulkanEngine* Engine = CRE_Globals::GetEnginePointer();
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

void CRE_Texture::OnRename()
{
	//???? Should we unload and reload here?
}
