#include "CRE_App.hpp"

//std incl
#include <filesystem>
#include <array>

//glm graphics incl
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


struct SimplePushConstantData
{
	glm::mat2 Transform{ 1.f };
	glm::vec2 Offset;
	alignas(16) glm::vec3 Color;
};

CRE_App::CRE_App()
{
	Window = new CRE_Window(WIDTH, HEIGHT, "New Vulkan Window");

	Device = new CRE_Device(*Window);

	LoadGameObjects();

	//Order is important here. Must happen after the previous pointers have been initialized.
	CreatePipelineLayout();
	RecreateSwapChain();
	CreateCommandBuffers();
}

CRE_App::~CRE_App()
{
	//Required here since initialization order must be followed in reverse when deleting.
	delete GraphicsPipeline;

	vkDestroyPipelineLayout(Device->device(), PipelineLayout, nullptr);

	GameObjects.clear();

	SwapChain.reset();//must destroy swapchain before device. This is dumb, but I don't care at the moment.
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

void CRE_App::LoadGameObjects()
{
	std::vector<CRE_Mesh::Vertex> Verticies
	{
		{{0.5f, -0.5f}	,{1.f,0.f,0.f}},
		{{1.0f, 0.5f}	,{0.f,1.f,0.f}},
		{{-0.5f, 0.5f}	,{0.f,0.f,1.f}},
	};

	auto Mesh = std::make_shared<CRE_Mesh>(Device, Verticies);

	CRE_PhysicalGameObject Triangle = CRE_PhysicalGameObject::CreateGameObject();
	Triangle.MeshObject = Mesh;
	Triangle.Transform.Translation.x = .2f;
	Triangle.Transform.Scale = {2.f, .5f};
	Triangle.Transform.Rotation = 0.25f * glm::two_pi<float>();

	GameObjects.push_back(std::move(Triangle));
}

void CRE_App::CreatePipelineLayout()
{
	VkPushConstantRange PushConstantRange;
	PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	PushConstantRange.offset = 0;
	PushConstantRange.size = sizeof(SimplePushConstantData);

	VkPipelineLayoutCreateInfo PipelineLayoutInfo{};
	PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	PipelineLayoutInfo.setLayoutCount = 0;
	PipelineLayoutInfo.pSetLayouts = nullptr;
	PipelineLayoutInfo.pushConstantRangeCount = 1;
	PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;

	if (vkCreatePipelineLayout(Device->device(), &PipelineLayoutInfo, nullptr, &PipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Pipeline Layout.");
	}
}

void CRE_App::CreatePipeline()
{
	assert(SwapChain != nullptr && "Missing swap chain!");
	assert(PipelineLayout != nullptr && "Missing pipeline layout!");

	//Delete old pipeline.
	if (GraphicsPipeline != nullptr)
	{
		delete GraphicsPipeline;
	}

	//calling the static default function is not required, since it is called in the constructor now.
	CRE_PipelineConfigInfo PipelineConfig{};

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
}

void CRE_App::FreeCommandBuffers()
{
	vkFreeCommandBuffers(Device->device(), Device->getCommandPool(), static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
	CommandBuffers.clear();
}

void CRE_App::DrawFrame()
{
	uint32_t ImageIndex;
	auto Result = SwapChain->acquireNextImage(&ImageIndex);

	if (Result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}

	if (Result != VK_SUCCESS && Result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to aquire swap chain image.");
	}

	RecordCommandBuffer(ImageIndex);
	Result = SwapChain->submitCommandBuffers(&CommandBuffers[ImageIndex], &ImageIndex);

	if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR || Window->WasWindowResized())
	{
		RecreateSwapChain();
		return;
	}

	if (Result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit command buffers.");
	}
}

void CRE_App::RecreateSwapChain()
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
		SwapChain = std::make_shared<CRE_Swap_Chain>(*Device, Extent, std::move(SwapChain));

		if (SwapChain->imageCount() != CommandBuffers.size())
		{
			FreeCommandBuffers();
			CreateCommandBuffers();
		}
	}

	CreatePipeline();
}

void CRE_App::RecordCommandBuffer(int ImageIndex)
{
	VkCommandBufferBeginInfo BeginInfo{};
	BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	if (vkBeginCommandBuffer(CommandBuffers[ImageIndex], &BeginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo RenderPassInfo{};
	RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	RenderPassInfo.renderPass = SwapChain->getRenderPass();
	RenderPassInfo.framebuffer = SwapChain->getFrameBuffer(ImageIndex);

	RenderPassInfo.renderArea.offset = { 0,0 };
	RenderPassInfo.renderArea.extent = SwapChain->getSwapChainExtent();

	std::array<VkClearValue, 2> ClearValues{};
	ClearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.f };
	ClearValues[1].depthStencil = { 1.f, 0 };

	RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
	RenderPassInfo.pClearValues = ClearValues.data();

	vkCmdBeginRenderPass(CommandBuffers[ImageIndex], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	VkViewport Viewport{};
	Viewport.x = 0.f;
	Viewport.y = 0.f;
	Viewport.width = static_cast<float>(SwapChain->getSwapChainExtent().width);
	Viewport.height = static_cast<float>(SwapChain->getSwapChainExtent().height);
	Viewport.minDepth = 0.f;
	Viewport.maxDepth = 1.f;
	VkRect2D Scissor{ {0, 0}, SwapChain->getSwapChainExtent()};
	vkCmdSetViewport(CommandBuffers[ImageIndex], 0, 1, &Viewport);
	vkCmdSetScissor(CommandBuffers[ImageIndex], 0, 1, &Scissor);

	RenderGameObjects(CommandBuffers[ImageIndex]);

	vkCmdEndRenderPass(CommandBuffers[ImageIndex]);
	if (vkEndCommandBuffer(CommandBuffers[ImageIndex]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to end command buffer.");
	}
}

void CRE_App::RenderGameObjects(VkCommandBuffer CommandBuffer)
{
	GraphicsPipeline->Bind(CommandBuffer);
	for (auto& Elem : GameObjects)
	{
		Elem.Transform.Rotation = glm::mod(Elem.Transform.Rotation + 0.01f, glm::two_pi<float>());

		SimplePushConstantData Push;
		Push.Offset = Elem.Transform.Translation;
		Push.Color = Elem.Color;
		Push.Transform = Elem.Transform;

		vkCmdPushConstants(
			CommandBuffer,
			PipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			sizeof(SimplePushConstantData),
			&Push);

		Elem.MeshObject->Bind(CommandBuffer);
		Elem.MeshObject->Draw(CommandBuffer);
	}
}

