#pragma once

#include "CRE_Window.hpp"
#include "CRE_GraphicsPipeline.hpp"

class CRE_App
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;

	CRE_App();
	~CRE_App();

	void Run();

private:

	CRE_Window* Window = nullptr;
	CRE_GraphicsPipeline* GraphicsPipeline = nullptr;
};
