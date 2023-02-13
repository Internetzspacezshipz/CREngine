#pragma once

#include "CRE_Device.hpp"
#include "CRE_GraphicsPipeline.hpp"
#include "CRE_PhysicalGameObject.hpp"

//std incl
#include <vector>
#include <memory>

class CRE_RenderSystem
{
public:
	CRE_RenderSystem(CRE_Device* InDevice, VkRenderPass InRenderPass);
	~CRE_RenderSystem();

	CRE_RenderSystem(const CRE_RenderSystem&) = delete;
	CRE_RenderSystem& operator= (const CRE_RenderSystem&) = delete;

	void RenderGameObjects(VkCommandBuffer CommandBuffer, std::vector<CRE_PhysicalGameObject>& GameObjects);

private:
	void CreatePipelineLayout();
	void CreatePipeline(VkRenderPass RenderPass);

	CRE_Device* Device = nullptr;
	CRE_GraphicsPipeline* GraphicsPipeline;
	VkPipelineLayout PipelineLayout;
};
