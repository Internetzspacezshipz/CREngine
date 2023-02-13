#pragma once

#include "CRE_Window.hpp"
#include "CRE_Device.hpp"
#include "CRE_Swap_Chain.hpp"

//std incl
#include <vector>
#include <memory>
#include <assert.h>

class CRE_Renderer
{
public:
	CRE_Renderer(CRE_Window* NewWindow, CRE_Device* NewDevice);
	~CRE_Renderer();

	CRE_Renderer(const CRE_Renderer&) = delete;
	CRE_Renderer& operator= (const CRE_Renderer&) = delete;

	VkRenderPass GetSwapChainRenderPass() const { return SwapChain->getRenderPass(); }

	bool GetIsFrameStarted() const { return bIsFrameStarted; }

	bool GetCurrentCommandBuffer(VkCommandBuffer& OutCommandBuffer) const 
	{ 
		if (GetIsFrameStarted())
		{
			OutCommandBuffer = CommandBuffers[CurrentFrameIndex];
			return true;
		}
		return false;
	}

	int GetFrameIndex() const
	{ 
		assert(bIsFrameStarted && "Cannot get frame index when frame is not in progress!");
		return CurrentFrameIndex;
	}

	VkCommandBuffer BeginFrame();
	void EndFrame();

	void BeginSwapChainRenderPass(VkCommandBuffer CommandBuffer);
	void EndSwapChainRenderPass(VkCommandBuffer CommandBuffer);

private:
	void CreateCommandBuffers();
	void FreeCommandBuffers();

	void RecreateSwapChain();


	CRE_Window* Window = nullptr;
	CRE_Device* Device = nullptr;
	std::shared_ptr<CRE_Swap_Chain> SwapChain = nullptr;
	VkPipelineLayout PipelineLayout;
	std::vector<VkCommandBuffer> CommandBuffers;

	uint32_t CurrentImageIndex;
	int CurrentFrameIndex;
	bool bIsFrameStarted;
};
