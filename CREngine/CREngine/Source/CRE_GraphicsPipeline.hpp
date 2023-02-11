#pragma once

#include "CRE_Device.hpp"

//std incl
#include <string>
#include <vector>

struct CRE_PipelineConfigInfo
{
	static CRE_PipelineConfigInfo Default(uint32_t InWidth, uint32_t InHeight);

	VkViewport viewport;
	VkRect2D scissor;
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
	VkPipelineRasterizationStateCreateInfo rasterizationInfo;
	VkPipelineMultisampleStateCreateInfo multisampleInfo;
	VkPipelineColorBlendAttachmentState colorBlendAttachment;
	VkPipelineColorBlendStateCreateInfo colorBlendInfo;
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
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
	void operator=(const CRE_GraphicsPipeline&) = delete;

	

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