#include "CRE_Window.hpp"

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

inline bool CRE_Window::ShouldClose()
{
	return glfwWindowShouldClose(Window);
}

void CRE_Window::InitWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	Window = glfwCreateWindow(Width, Height, WindowName.c_str(), nullptr, nullptr);
}