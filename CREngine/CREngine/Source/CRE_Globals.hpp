#pragma once

//Holds any global pointers for anything to get.

namespace CRE_Globals
{
	static class VulkanEngine*& GetEnginePointer()
	{
		static VulkanEngine* Engine;
		return Engine;
	}
}
