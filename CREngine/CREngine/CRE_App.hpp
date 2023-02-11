#pragma once

#include "CRE_Window.hpp"

class CRE_App
{
public:
	static constexpr int WIDTH = 800;
	static constexpr int HEIGHT = 600;
	CRE_App() 
	{
		Window = new CRE_Window(WIDTH, HEIGHT, "New Vulkan Window");
	};
	~CRE_App() 
	{
		delete Window;
	};
	void Run();

private:

	CRE_Window* Window = nullptr;
};
