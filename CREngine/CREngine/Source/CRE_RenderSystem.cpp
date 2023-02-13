#include "CRE_RenderSystem.hpp"

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
	glm::mat4 Transform{ 1.f };
	alignas(16) glm::vec3 Color;
};

CRE_RenderSystem::CRE_RenderSystem(CRE_Device* InDevice, VkRenderPass InRenderPass)
	: Device(InDevice)
{
	CreatePipelineLayout();
	CreatePipeline(InRenderPass);
}

CRE_RenderSystem::~CRE_RenderSystem()
{
	//Required here since initialization order must be followed in reverse when deleting.
	delete GraphicsPipeline;

	vkDestroyPipelineLayout(Device->device(), PipelineLayout, nullptr);
}

void CRE_RenderSystem::CreatePipelineLayout()
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

void CRE_RenderSystem::CreatePipeline(VkRenderPass RenderPass)
{
	assert(PipelineLayout != nullptr && "Missing pipeline layout!");

	//Delete old pipeline.
	if (GraphicsPipeline != nullptr)
	{
		delete GraphicsPipeline;
	}

	//calling the static default function is not required, since it is called in the constructor now.
	CRE_PipelineConfigInfo PipelineConfig{};

	PipelineConfig.renderPass = RenderPass;
	PipelineConfig.pipelineLayout = PipelineLayout;

	auto BasePath = std::filesystem::current_path();
	auto VPath = BasePath / std::filesystem::path("Shaders\\Compiled\\Simple_VertShader.vert.spv");
	auto FPath = BasePath / std::filesystem::path("Shaders\\Compiled\\Simple_FragShader.frag.spv");

	GraphicsPipeline = new CRE_GraphicsPipeline(*Device, PipelineConfig, VPath.string(), FPath.string());
}

void CRE_RenderSystem::RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<CRE_PhysicalGameObject>& GameObjects)
{
	GraphicsPipeline->Bind(CommandBuffer);
	for (auto& Elem : GameObjects)
	{
		auto& Transform = Elem.Transform;

		auto Euler = Transform.GetRotationAsEuler();
		
		//todo: make rotations not dumb.
		Euler.x = fmod(Euler.x + 0.00001f, 3.14/4);
		Euler.y = fmod(Euler.y + 0.00001f, 3.14/4);

		Transform.SetRotationFromEuler(Euler);

		SimplePushConstantData Push;
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