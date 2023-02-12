#pragma once

#include "CRE_Window.hpp"
#include "CRE_Device.hpp"
#include "CRE_Swap_Chain.hpp"
#include "CRE_GraphicsPipeline.hpp"

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

	void CreatePipelineLayout();
	void CreatePipeline();
	void CreateCommandBuffers();
	void DrawFrame();

	CRE_Window* Window = nullptr;
	CRE_Device* Device = nullptr;
	CRE_Swap_Chain* SwapChain = nullptr;
	CRE_GraphicsPipeline* GraphicsPipeline;
	VkPipelineLayout PipelineLayout;
	std::vector<VkCommandBuffer> CommandBuffers;
};
