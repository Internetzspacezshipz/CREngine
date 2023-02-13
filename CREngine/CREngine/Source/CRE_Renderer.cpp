#include "CRE_Renderer.hpp"

//std incl
#include <filesystem>
#include <array>

CRE_Renderer::CRE_Renderer(CRE_Window* NewWindow, CRE_Device* NewDevice) : 
	Window(NewWindow),
	Device(NewDevice)
{
	//Order is important here. Must happen after the previous pointers have been initialized.
	RecreateSwapChain();
	CreateCommandBuffers();
}

CRE_Renderer::~CRE_Renderer()
{
	FreeCommandBuffers();
	SwapChain.reset();//must destroy swapchain before device. This is dumb, but I don't care at the moment.
}

VkCommandBuffer CRE_Renderer::BeginFrame()
{
	assert(!bIsFrameStarted && "Cannot begin frame when another is still active.");


	auto Result = SwapChain->acquireNextImage(&CurrentImageIndex);

	if (Result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return nullptr;
	}

	if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to aquire swap chain image.");
	}

	bIsFrameStarted = true;

	VkCommandBuffer OutCB;
	if (GetCurrentCommandBuffer(OutCB))
	{
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(OutCB, &BeginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		return OutCB;
	}
	return nullptr;
}

void CRE_Renderer::EndFrame()
{
	assert(bIsFrameStarted && "Cannot call endframe while no frame in progress");
	VkCommandBuffer OutCB;
	if (GetCurrentCommandBuffer(OutCB))
	{
		if (vkEndCommandBuffer(OutCB) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end command buffer.");
		}

		auto Result = SwapChain->submitCommandBuffers(&OutCB, &CurrentImageIndex);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || Window->WasWindowResized())
		{
			RecreateSwapChain();
		}
		else if (Result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit command buffers.");
		}

		bIsFrameStarted = false;
		CurrentFrameIndex = (CurrentFrameIndex + 1) % CRE_Swap_Chain::MAX_FRAMES_IN_FLIGHT;
	}
}

void CRE_Renderer::BeginSwapChainRenderPass(VkCommandBuffer CommandBuffer)
{
	assert(bIsFrameStarted && "cannot call BeginSwapChainRenderPass while frame not in progress");
	VkCommandBuffer Buffer;
	if (GetCurrentCommandBuffer(Buffer))
	{
		assert(CommandBuffer == Buffer && "Cannot call BeginSwapChainRenderPass on command buffer from another frame.");
	}

	VkRenderPassBeginInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassInfo.renderPass = SwapChain->getRenderPass();
	RenderPassInfo.framebuffer = SwapChain->getFrameBuffer(CurrentImageIndex);

	RenderPassInfo.renderArea.offset = { 0,0 };
	RenderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> ClearValues{};
	ClearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.f };
	ClearValues[1].depthStencil = { 1.f, 0 };

	RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
	RenderPassInfo.pClearValues = ClearValues.data();

	vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport Viewport{};
	Viewport.x = 0.f;
	Viewport.y = 0.f;
	Viewport.width = static_cast<float>(SwapChain->getSwapChainExtent().width);
	Viewport.height = static_cast<float>(SwapChain->getSwapChainExtent().height);
	Viewport.minDepth = 0.f;
	Viewport.maxDepth = 1.f;
	VkRect2D Scissor{ {0, 0}, SwapChain->getSwapChainExtent() };
	vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
	vkCmdSetScissor(CommandBuffer, 0, 1, &Scissor);
}

void CRE_Renderer::EndSwapChainRenderPass(VkCommandBuffer CommandBuffer)
{
	assert(bIsFrameStarted && "cannot call EndSwapChainRenderPass while frame not in progress");
	VkCommandBuffer Buffer;
	if (GetCurrentCommandBuffer(Buffer))
	{
		assert(CommandBuffer == Buffer && "Cannot call EndSwapChainRenderPass on command buffer from another frame.");
	}

	vkCmdEndRenderPass(CommandBuffer);
}

void CRE_Renderer::CreateCommandBuffers()
{
	CommandBuffers.resize(CRE_Swap_Chain::MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocInfo.commandPool = Device->getCommandPool();
	AllocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

	if (vkAllocateCommandBuffers(Device->device(), &AllocInfo, CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to alloc command buffers!");
	}
}

void CRE_Renderer::FreeCommandBuffers()
{
	vkFreeCommandBuffers(Device->device(), Device->getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	CommandBuffers.clear();
}

void CRE_Renderer::RecreateSwapChain()
{
	auto Extent = Window->GetExtent();
	while (Extent.width == 0 || Extent.height == 0)
	{
		Extent = Window->GetExtent();
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(Device->device());

	if (SwapChain == nullptr)
	{
		SwapChain = std::make_shared<CRE_Swap_Chain>(*Device, Extent);
	}
	else
	{
		std::shared_ptr<CRE_Swap_Chain> OldSwapChain = std::move(SwapChain);

		SwapChain = std::make_shared<CRE_Swap_Chain>(*Device, Extent, OldSwapChain);

		if (!OldSwapChain->compareSwapFormats(*SwapChain.get()))
		{
			throw std::runtime_error("Swap chain format has changed!");
		}
	}

	//todo;
}
