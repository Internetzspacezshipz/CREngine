#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

class CRE_Window
{
public:
	CRE_Window(int InWidth, int InHeight, std::string Name);
	~CRE_Window();

	//delete the copy constructors.
	CRE_Window(const CRE_Window&) = delete;
	CRE_Window& operator= (const CRE_Window&) = delete;

	bool ShouldClose();

	void CreateWindowSurface(VkInstance Instance, VkSurfaceKHR* Surface);

private:
	void InitWindow();

	const int Width;
	const int Height;

	std::string WindowName;
	GLFWwindow* Window;
};
