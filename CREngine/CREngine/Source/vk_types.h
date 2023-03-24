// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan/vulkan.h>

//Whether we should record VMA to look for memory leaks and such.
#define VMA_REC 1

#ifndef VMA_RECORDING_ENABLED
#define VMA_RECORDING_ENABLED VMA_REC
#endif

#include "vk_mem_alloc.h"

typedef uint64_t AssetHandle;

struct AllocatedBuffer
{
	VkBuffer _buffer;
	VmaAllocation _allocation;
};

struct AllocatedImage
{
	int texWidth;
	int texHeight;
	int texChannelsActual;
	int texChannelsCreated;

	VkImage _image;
	VmaAllocation _allocation;
};