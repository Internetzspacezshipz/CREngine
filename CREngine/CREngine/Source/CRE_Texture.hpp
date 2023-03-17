#pragma once

#include "CRE_Serialization.hpp"

//Enable STB image implementation.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "boost/filesystem.hpp"

//Handles a 2D texture and loading it.
class CRE_Texture : public CRE_SerializerInterface
{
	boost::filesystem::path Path;
	//pixel array.
	stbi_uc* Data;
	//output data about size.
	int ImageSize;
	//W/H/Channels
	int TexWidth;
	int TexHeight;
	int TexChannels;

public:

	~CRE_Texture();

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	bool Load();
	bool SendToGPU();
};

