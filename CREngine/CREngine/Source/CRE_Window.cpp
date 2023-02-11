#include "CRE_Window.hpp"
#include <stdexcept>

CRE_Window::CRE_Window(int InWidth, int InHeight, std::string NewWindowName) :
	Width(InWidth), Height(InHeight), WindowName(NewWindowName)
{
	InitWindow();
}

CRE_Window::~CRE_Window()
{
	glfwDestroyWindow(Window);
	glfwTerminate();
}

bool CRE_Window::ShouldClose()
{
	return glfwWindowShouldClose(Window);
}

void CRE_Window::CreateWindowSurface(VkInstance Instance, VkSurfaceKHR* Surface)
{
	if (glfwCreateWindowSurface(Instance, Window, nullptr, Surface) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create window surface.");
	}
}

void CRE_Window::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	Window = glfwCreateWindow(Width, Height, WindowName.c_str(), nullptr, nullptr);
}