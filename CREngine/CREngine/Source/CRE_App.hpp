#pragma once

#include "CRE_Window.hpp"
#include "CRE_Device.hpp"
#include "CRE_Swap_Chain.hpp"
#include "CRE_GraphicsPipeline.hpp"
#include "CRE_PhysicalGameObject.hpp"

//std incl
#include <vector>
#include <memory>

class CRE_App
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	CRE_App();
	~CRE_App();

	CRE_App(const CRE_App&) = delete;
	CRE_App& operator= (const CRE_App&) = delete;

	void Run();

private:
	void LoadGameObjects();
	void CreatePipelineLayout();
	void CreatePipeline();
	void CreateCommandBuffers();
	void FreeCommandBuffers();
	void DrawFrame();

	void RecreateSwapChain();
	void RecordCommandBuffer(int ImageIndex);
	void RenderGameObjects(VkCommandBuffer CommandBuffer);

	//What a clusterfuck.

	CRE_Window* Window = nullptr;
	CRE_Device* Device = nullptr;
	std::shared_ptr<CRE_Swap_Chain> SwapChain = nullptr;
	CRE_GraphicsPipeline* GraphicsPipeline;
	VkPipelineLayout PipelineLayout;
	std::vector<VkCommandBuffer> CommandBuffers;

	std::vector<CRE_PhysicalGameObject> GameObjects;
};
