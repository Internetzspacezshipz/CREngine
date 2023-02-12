#include "CRE_GraphicsPipeline.hpp"
#include "CRE_Mesh.hpp"

#include <fstream>
#include <iostream>
#include <cassert>

CRE_PipelineConfigInfo CRE_PipelineConfigInfo::Default(uint32_t InWidth, uint32_t InHeight)
{
    CRE_PipelineConfigInfo configInfo{};

    configInfo.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    configInfo.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    configInfo.viewport.x = 0.0f;
    configInfo.viewport.y = 0.0f;
    configInfo.viewport.width = static_cast<float>(InWidth);
    configInfo.viewport.height = static_cast<float>(InHeight);
    configInfo.viewport.minDepth = 0.0f;
    configInfo.viewport.maxDepth = 1.0f;

    configInfo.scissor.offset = { 0, 0 };
    configInfo.scissor.extent = { InWidth, InHeight };

    configInfo.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    configInfo.rasterizationInfo.depthClampEnable = VK_FALSE;
    configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    configInfo.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    configInfo.rasterizationInfo.lineWidth = 1.0f;
    configInfo.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    configInfo.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    configInfo.rasterizationInfo.depthBiasEnable = VK_FALSE;
    configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    configInfo.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    configInfo.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    configInfo.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    configInfo.multisampleInfo.sampleShadingEnable = VK_FALSE;
    configInfo.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    configInfo.multisampleInfo.minSampleShading = 1.0f;           // Optional
    configInfo.multisampleInfo.pSampleMask = nullptr;             // Optional
    configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    configInfo.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    configInfo.colorBlendAttachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT |
        VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    configInfo.colorBlendAttachment.blendEnable = VK_FALSE;
    configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    configInfo.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    configInfo.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    configInfo.colorBlendInfo.logicOpEnable = VK_FALSE;
    configInfo.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    configInfo.colorBlendInfo.attachmentCount = 1;
    configInfo.colorBlendInfo.pAttachments = &configInfo.colorBlendAttachment;
    configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

    configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    configInfo.depthStencilInfo.depthTestEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthWriteEnable = VK_TRUE;
    configInfo.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    configInfo.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    configInfo.depthStencilInfo.stencilTestEnable = VK_FALSE;
    configInfo.depthStencilInfo.front = {};  // Optional
    configInfo.depthStencilInfo.back = {};   // Optional

    return configInfo;
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

    VkPipelineViewportStateCreateInfo ViewportInfo{};
    ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    ViewportInfo.viewportCount = 1;
    ViewportInfo.pViewports = &PipelineConfig.viewport;
    ViewportInfo.scissorCount = 1;
    ViewportInfo.pScissors = &PipelineConfig.scissor;

    VkGraphicsPipelineCreateInfo PipelineInfo{};
    PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    PipelineInfo.stageCount = 2;
    PipelineInfo.pStages = ShaderStages;
    PipelineInfo.pVertexInputState = &VertexInputInfo;
    PipelineInfo.pInputAssemblyState = &PipelineConfig.inputAssemblyInfo;
    PipelineInfo.pViewportState = &ViewportInfo;
    PipelineInfo.pRasterizationState = &PipelineConfig.rasterizationInfo;
    PipelineInfo.pMultisampleState = &PipelineConfig.multisampleInfo;
    PipelineInfo.pColorBlendState = &PipelineConfig.colorBlendInfo;
    PipelineInfo.pDynamicState = nullptr; //optional

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
