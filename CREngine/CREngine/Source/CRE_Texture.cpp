#include "CRE_Texture.hpp"

CRE_Texture::~CRE_Texture()
{
	//Ensure we delete data when the texture is unloaded.
	delete Data;
}

void CRE_Texture::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Path);
	//For now, let's just load the image when we're deserializing.
	if (!bSerializing)
	{
		Load();
	}
}

bool CRE_Texture::Load()
{
	if (!Path.empty())
	{
		Data = stbi_load(Path.generic_string().c_str(), &TexWidth, &TexHeight, &TexChannels, STBI_rgb_alpha);

		//Total Data size is 4 bytes/pixel * height * width.
		ImageSize = TexWidth * TexHeight * 4;

		if (!Data)
		{
			return false;
		}

		return true;
	}
	return false;
}
