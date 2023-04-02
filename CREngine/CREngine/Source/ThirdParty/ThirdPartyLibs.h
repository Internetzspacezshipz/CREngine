#pragma once

//Include this file in order to include third party libs instead of directly including them.

//Add conversions for ImStrv
#define IM_STRV_CLASS_EXTRA \
ImStrv(const std::string_view& s)  { Begin = s.data(); End = Begin + s.length(); }\
ImStrv(const std::string& s)    { Begin = s.c_str(); End = Begin + s.length(); }

#include <ThirdParty/imgui/imgui.h>
#include <ThirdParty/imgui/imgui_stdlib.h>
#include <ThirdParty/imgui/imgui_impl_sdl2.h>
#include <ThirdParty/imgui/imgui_impl_vulkan.h>

//Function type
#include <ThirdParty/func/func.hpp>

//json serialization:
#include <json.hpp>

//Math
#include <glm/glm.hpp>

//Vulkan bootstrap for helping with managment of swapchain and other vulkan parts.
#include "thirdparty/vkbootstrap/VkBootstrap.h"

//Vulkan descriptor allocator https://github.com/vblanco20-1/Vulkan-Descriptor-Allocator
#include <thirdparty/descriptorAlloc/descriptor_allocator.h>
