// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vk_engine.h>

namespace vkutil {

	bool load_image_from_file(VulkanEngine* engine, const char* file, AllocatedImage& outImage);

	bool AllocImage(
		VulkanEngine* engine,
		const BinArray& ImageData,
		int TextureWidth, 
		int TextureHeight, 
		int TextureChannels, 
		VkFormat image_format,
		AllocatedImage& outImage);
}