#include "CRE_GraphicsPipeline.hpp"
#include "CRE_Mesh.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

void CRE_PipelineConfigInfo::Default(CRE_PipelineConfigInfo& Info)
{
    Info.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    Info.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    Info.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
    Info.inputAssemblyInfo.pNext = nullptr;
    Info.inputAssemblyInfo.flags = 0;

    Info.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    Info.ViewportInfo.viewportCount = 1;
    Info.ViewportInfo.pViewports = nullptr;
    Info.ViewportInfo.scissorCount = 1;
    Info.ViewportInfo.pScissors = nullptr;
    Info.ViewportInfo.pNext = nullptr;
    Info.ViewportInfo.flags = 0;

    Info.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    Info.rasterizationInfo.depthClampEnable = VK_FALSE;
    Info.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    Info.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    Info.rasterizationInfo.lineWidth = 1.0f;
    Info.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    Info.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    Info.rasterizationInfo.depthBiasEnable = VK_FALSE;
    Info.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    Info.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    Info.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional
    Info.rasterizationInfo.pNext = nullptr;
    Info.rasterizationInfo.flags = 0;

    Info.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    Info.multisampleInfo.sampleShadingEnable = VK_FALSE;
    Info.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    Info.multisampleInfo.minSampleShading = 1.0f;           // Optional
    Info.multisampleInfo.pSampleMask = nullptr;             // Optional
    Info.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    Info.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    Info.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    Info.colorBlendAttachment.blendEnable = VK_FALSE;
    Info.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    Info.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    Info.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    Info.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    Info.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    Info.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    Info.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    Info.colorBlendInfo.logicOpEnable = VK_FALSE;
    Info.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    Info.colorBlendInfo.attachmentCount = 1;
    Info.colorBlendInfo.pAttachments = &Info.colorBlendAttachment;
    Info.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    Info.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    Info.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    Info.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

    Info.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    Info.depthStencilInfo.depthTestEnable = VK_TRUE;
    Info.depthStencilInfo.depthWriteEnable = VK_TRUE;
    Info.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    Info.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    Info.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    Info.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    Info.depthStencilInfo.stencilTestEnable = VK_FALSE;
    Info.depthStencilInfo.front = {};  // Optional
    Info.depthStencilInfo.back = {};   // Optional

    Info.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    Info.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    Info.DynamicStateInfo.pDynamicStates = Info.DynamicStateEnables.data();
    Info.DynamicStateInfo.dynamicStateCount = static_cast<uint32_t>(Info.DynamicStateEnables.size());
    Info.DynamicStateInfo.flags = 0;
}

CRE_GraphicsPipeline::CRE_GraphicsPipeline(CRE_Device& Device,
    const CRE_PipelineConfigInfo& PipelineConfig,
    const std::string& VertShaderFilePath,
    const std::string& FragShaderFilePath)
    : OwnedDevice(Device)
{
    CreateGraphicsPipeline(PipelineConfig, VertShaderFilePath, FragShaderFilePath);
}

CRE_GraphicsPipeline::~CRE_GraphicsPipeline()
{
    vkDestroyShaderModule(OwnedDevice.device(), VertShaderModule, nullptr);
    vkDestroyShaderModule(OwnedDevice.device(), FragShaderModule, nullptr);
    vkDestroyPipeline(OwnedDevice.device(), GraphicsPipeline, nullptr);
}

void CRE_GraphicsPipeline::Bind(VkCommandBuffer CommandBuffer)
{
    vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicsPipeline);
}

std::vector<char> CRE_GraphicsPipeline::ReadFile(const std::string& FilePath)
{
    std::ifstream File(FilePath, std::ios::ate | std::ios::binary);

    if (!File.is_open())
    {
        throw std::runtime_error("Failed to open file: +" + FilePath);
        return std::vector<char>();
    }

    size_t FileSize = static_cast<size_t>(File.tellg());

    std::vector<char> FileBuffer(FileSize);

    File.seekg(0);
    File.read(FileBuffer.data(), FileSize);

    File.close();
    return FileBuffer;
}

void CRE_GraphicsPipeline::CreateShaderModule(const std::vector<char>& ShaderCode, VkShaderModule* TargetShaderModule)
{
    VkShaderModuleCreateInfo CreateInfo{};
    CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    CreateInfo.codeSize = ShaderCode.size();
    CreateInfo.pCode = reinterpret_cast<const uint32_t*>(ShaderCode.data());
    CreateInfo.pNext = nullptr;

    if (vkCreateShaderModule(OwnedDevice.device(), &CreateInfo, nullptr, TargetShaderModule) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create shader module");
    }
}

void CRE_GraphicsPipeline::CreateGraphicsPipeline(const CRE_PipelineConfigInfo& PipelineConfig, const std::string& VertShaderFilePath, const std::string& FragShaderFilePath)
{
    assert(PipelineConfig.pipelineLayout != VK_NULL_HANDLE && "Cannot create graphics pipeline.");
    assert(PipelineConfig.renderPass != VK_NULL_HANDLE && "Cannot create graphics pipeline (render pass).");

    auto VertCode = ReadFile(VertShaderFilePath);
    auto FragCode = ReadFile(FragShaderFilePath);

    CreateShaderModule(VertCode, &VertShaderModule);
    CreateShaderModule(FragCode, &FragShaderModule);

    VkPipelineShaderStageCreateInfo ShaderStages[2];

    ShaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ShaderStages[0].module = VertShaderModule;
    ShaderStages[0].pName = "main";
    ShaderStages[0].flags = 0;
    ShaderStages[0].pNext = nullptr;
    ShaderStages[0].pSpecializationInfo = nullptr;

    ShaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStages[1].module = FragShaderModule;
    ShaderStages[1].pName = "main";
    ShaderStages[1].flags = 0;
    ShaderStages[1].pNext = nullptr;
    ShaderStages[1].pSpecializationInfo = nullptr;

    auto AttributeDescriptions = CRE_Mesh::Vertex::GetAttributeDescriptions();
    auto BindingDescriptions = CRE_Mesh::Vertex::GetBindingDescriptions();

    VkPipelineVertexInputStateCreateInfo VertexInputInfo{};
    VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(AttributeDescriptions.size());
    VertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(BindingDescriptions.size());
    VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions.data();
    VertexInputInfo.pVertexBindingDescriptions = BindingDescriptions.data();

    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = 2;
    PipelineInfo.pStages = ShaderStages;
    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &PipelineConfig.inputAssemblyInfo;
    PipelineInfo.pViewportState = &PipelineConfig.ViewportInfo;
    PipelineInfo.pRasterizationState = &PipelineConfig.rasterizationInfo;
    PipelineInfo.pMultisampleState = &PipelineConfig.multisampleInfo;
    PipelineInfo.pColorBlendState = &PipelineConfig.colorBlendInfo;
    PipelineInfo.pDepthStencilState = &PipelineConfig.depthStencilInfo;

    PipelineInfo.pDynamicState = &PipelineConfig.DynamicStateInfo;

    PipelineInfo.layout = PipelineConfig.pipelineLayout;
    PipelineInfo.renderPass = PipelineConfig.renderPass;
    PipelineInfo.subpass = PipelineConfig.subpass;


    PipelineInfo.basePipelineIndex = -1;
    PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(OwnedDevice.device(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &GraphicsPipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create graphics pipeline");
    }
}
