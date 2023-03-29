// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vulkan/vulkan.h>

//Whether we should record VMA to look for memory leaks and such.
#define VMA_REC 1

#ifndef VMA_RECORDING_ENABLED
#define VMA_RECORDING_ENABLED VMA_REC
#endif

//Due to vk_mem_alloc.h including Windows.h
//we end up with a collision between the #define min/max and std::min/max in some annoying file
//So define NOMINMAX to avoid this crap.
#define NOMINMAX
#include "vk_mem_alloc.h"

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