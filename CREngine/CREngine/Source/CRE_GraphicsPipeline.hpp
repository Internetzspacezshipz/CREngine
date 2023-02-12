#pragma once

#include "CRE_Device.hpp"

//std incl
#include <string>
#include <vector>

struct CRE_PipelineConfigInfo
{
	static void Default(CRE_PipelineConfigInfo& Info);

	CRE_PipelineConfigInfo()
	{
		//Clear memory, since certain fields not defined in the Vk structs need to be nullptr and zeroed.
		memset(this, 0, sizeof(CRE_PipelineConfigInfo));
		Default(*this);
	};

	CRE_PipelineConfigInfo(const CRE_PipelineConfigInfo&) = delete;
	CRE_PipelineConfigInfo& operator=(const CRE_PipelineConfigInfo&) = delete;

	VkPipelineViewportStateCreateInfo ViewportInfo;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;

	std::vector<VkDynamicState> DynamicStateEnables;
	VkPipelineDynamicStateCreateInfo DynamicStateInfo;

	VkPipelineLayout pipelineLayout = nullptr;
	VkRenderPass renderPass = nullptr;
	uint32_t subpass = 0;
};

class CRE_GraphicsPipeline
{
public:
	CRE_GraphicsPipeline(
		CRE_Device& Device,
		const CRE_PipelineConfigInfo& PipelineConfig, 
		const std::string& VertShaderFilePath, 
		const std::string& FragShaderFilePath);

	~CRE_GraphicsPipeline();

	CRE_GraphicsPipeline(const CRE_GraphicsPipeline&) = delete;
	CRE_GraphicsPipeline& operator=(const CRE_GraphicsPipeline&) = delete;

	void Bind(VkCommandBuffer CommandBuffer);
	
private:
	static std::vector<char> ReadFile(const std::string& FilePath);
	void CreateGraphicsPipeline(
		const CRE_PipelineConfigInfo& PipelineConfig, 
		const std::string& VertShaderFilePath,
		const std::string& FragShaderFilePath);

	void CreateShaderModule(const std::vector<char>& ShaderCode, VkShaderModule* TargetShaderModule);

	CRE_Device& OwnedDevice;
	VkPipeline GraphicsPipeline;
	VkShaderModule VertShaderModule;
	VkShaderModule FragShaderModule;
};