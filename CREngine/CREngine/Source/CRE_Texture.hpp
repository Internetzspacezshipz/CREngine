#pragma once

#include "CRE_Serialization.hpp"
#include "CRE_ManagedObject.hpp"

//Enable STB image implementation.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "boost/filesystem.hpp"

//Handles a 2D texture and loading it.
class CRE_Texture : public CRE_ManagedObject
{
	boost::filesystem::path Path;
	//pixel array.
	stbi_uc* Data;
	//output data about size.
	uint64_t ImageSize;
	//W/H/Channels
	int TexWidth;
	int TexHeight;
	int TexChannels;

	VkDeviceMemory ImageBufferMemory;

public:

	~CRE_Texture();

	// Inherited via CRE_SerializerInterface
	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	bool Load();
	bool SendToGPU(CRE_Device* Device);
};

