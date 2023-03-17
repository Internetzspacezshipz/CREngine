#include "CRE_Texture.hpp"
#include "vma.h"

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

bool CRE_Texture::SendToGPU(CRE_Device* Device)
{
	//the format R8G8B8A8 matches exactly with the pixels loaded from stb_image lib
	//VkFormat image_format = VK_FORMAT_R8G8B8A8_SRGB;

	//VkImageCreateInfo ImgCreateInfo;
	//ImgCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	//ImgCreateInfo.pNext = nullptr;
	//ImgCreateInfo.flags = 0;
	//ImgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	//ImgCreateInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
	//ImgCreateInfo.extent = ImageSize;
	//
	//Device->createImageWithInfo();
	//allocate temporary buffer for holding texture data to upload


	VkBuffer NewBuffer;
	Device->createBuffer(
		ImageSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		NewBuffer,
		ImageBufferMemory
	);


	//copy data to buffer
	void* data;
	vmaMapMemory(Device->getVmaAllocator(), stagingBuffer._allocation, &data);

	memcpy(data, pixel_ptr, static_cast<size_t>(imageSize));

	vmaUnmapMemory(engine._allocator, stagingBuffer._allocation);
	//we no longer need the loaded data, so we can free the pixels as they are now in the staging buffer
	stbi_image_free(pixels);

	return false;
}
