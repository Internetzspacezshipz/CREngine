#include "CRE_App.hpp"

#include <filesystem>
#include <array>

CRE_App::CRE_App()
{
	Window = new CRE_Window(WIDTH, HEIGHT, "New Vulkan Window");

	Device = new CRE_Device(*Window);

	SwapChain = new CRE_Swap_Chain(*Device, Window->GetExtent());

	LoadMeshes();

	//Order is important here. Must happen after the previous pointers have been initialized.
	CreatePipelineLayout();
	CreatePipeline();
	CreateCommandBuffers();
}

CRE_App::~CRE_App()
{
	//Required here since initialization order must be followed in reverse when deleting.
	delete GraphicsPipeline;

	vkDestroyPipelineLayout(Device->device(), PipelineLayout, nullptr);

	delete Mesh;
	delete SwapChain;
	delete Device;
	delete Window;
}

void CRE_App::Run()
{
	while (!Window->ShouldClose())
	{
		glfwPollEvents();
		DrawFrame();
	}

	//wait until vulkan has cleaned everything up.
	vkDeviceWaitIdle(Device->device());
}

void CRE_App::LoadMeshes()
{
	std::vector<CRE_Mesh::Vertex> Verticies
	{
		{{0.5f, -0.5f}},
		{{1.0f, 0.5f}},
		{{-0.5f, 0.5f}},

		{{-1.f, -0.5f}},
		{{-1.f, 0.f}},
		{{-0.5f, 0.5f}},
	};

	Mesh = new CRE_Mesh(Device, Verticies);
}

void CRE_App::CreatePipelineLayout()
{
	VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
	PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutInfo.setLayoutCount = 0;
	PipelineLayoutInfo.pSetLayouts = nullptr;
	PipelineLayoutInfo.pushConstantRangeCount = 0;
	PipelineLayoutInfo.pPushConstantRanges = nullptr;
	if (vkCreatePipelineLayout(Device->device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Pipeline Layout.");
	}
}

void CRE_App::CreatePipeline()
{
	auto PipelineConfig = CRE_PipelineConfigInfo::Default(SwapChain->width(), SwapChain->height());
	PipelineConfig.renderPass = SwapChain->getRenderPass();
	PipelineConfig.pipelineLayout = PipelineLayout;

	auto BasePath = std::filesystem::current_path();
	auto VPath = BasePath / std::filesystem::path("Shaders\\Compiled\\Simple_VertShader.vert.spv");
	auto FPath = BasePath / std::filesystem::path("Shaders\\Compiled\\Simple_FragShader.frag.spv");

	GraphicsPipeline = new CRE_GraphicsPipeline(*Device, PipelineConfig, VPath.string(), FPath.string());
}

void CRE_App::CreateCommandBuffers()
{
	CommandBuffers.resize(SwapChain->imageCount());

	VkCommandBufferAllocateInfo AllocInfo{};
	AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	AllocInfo.commandPool = Device->getCommandPool();
	AllocInfo.commandBufferCount = static_cast<uint32_t>(CommandBuffers.size());

	if (vkAllocateCommandBuffers(Device->device(), &AllocInfo, CommandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to alloc command buffers!");
	}

	for (int i = 0; i < CommandBuffers.size(); i++)
	{
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		if (vkBeginCommandBuffer(CommandBuffers[i], &BeginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo RenderPassInfo{};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = SwapChain->getRenderPass();
		RenderPassInfo.framebuffer = SwapChain->getFrameBuffer(i);

		RenderPassInfo.renderArea.offset = { 0,0 };
		RenderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

		std::array<VkClearValue, 2> ClearValues{};
		ClearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.f };
		ClearValues[1].depthStencil = { 1.f, 0 };

		RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
		RenderPassInfo.pClearValues = ClearValues.data();

		vkCmdBeginRenderPass(CommandBuffers[i], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		GraphicsPipeline->Bind(CommandBuffers[i]);
		Mesh->Bind(CommandBuffers[i]);
		Mesh->Draw(CommandBuffers[i]);

		vkCmdEndRenderPass(CommandBuffers[i]);
		if (vkEndCommandBuffer(CommandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to end command buffer.");
		}
	}
}

void CRE_App::DrawFrame()
{
	uint32_t ImageIndex;
	auto Result = SwapChain->acquireNextImage(&ImageIndex);

	if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to aquire swap chain image.");
	}

	Result = SwapChain->submitCommandBuffers(&CommandBuffers[ImageIndex], &ImageIndex);

	if (Result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit command buffers.");
	}
}

