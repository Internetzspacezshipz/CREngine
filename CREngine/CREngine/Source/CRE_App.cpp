#include "CRE_App.hpp"

#include <filesystem>

CRE_App::CRE_App()
{
	Window = new CRE_Window(WIDTH, HEIGHT, "New Vulkan Window");

	Device = new CRE_Device(*Window);

	SwapChain = new CRE_Swap_Chain(*Device, Window->GetExtent());

	//Order is important here. Must happen after the previous pointers have been initialized.
	CreatePipelineLayout();
	CreatePipeline();
	CreateCommandBuffers();
}

CRE_App::~CRE_App()
{
	//Required here since initialization order must be followed in reverse when deleting.
	GraphicsPipeline.release();

	vkDestroyPipelineLayout(Device->device(), PipelineLayout, nullptr);

	delete SwapChain;
	delete Device;
	delete Window;
}

void CRE_App::Run()
{
	while (!Window->ShouldClose())
	{
		glfwPollEvents();
	}
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

	GraphicsPipeline = std::make_unique<CRE_GraphicsPipeline>(*Device, PipelineConfig, VPath.string(), FPath.string());
}

void CRE_App::CreateCommandBuffers()
{
}

void CRE_App::DrawFrame()
{
}

