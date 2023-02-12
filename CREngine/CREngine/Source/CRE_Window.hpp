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

	VkExtent2D GetExtent() const { return VkExtent2D((uint32_t)Width, (uint32_t)Height); }

	bool WasWindowResized() const { return bFramebufferResized; }
	void ResetWindowResizedFlag() { bFramebufferResized = false; }
private:
	static void FramebufferResizedCallback(GLFWwindow* Window, int NewWidth, int NewHeight);
	void InitWindow();

	int Width;
	int Height;
	bool bFramebufferResized = false;

	std::string WindowName;
	GLFWwindow* Window;
};
