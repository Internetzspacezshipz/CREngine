#include "CRE_App.hpp"

void CRE_App::Run()
{
	while (!Window->ShouldClose())
	{
		glfwPollEvents();
	}
}