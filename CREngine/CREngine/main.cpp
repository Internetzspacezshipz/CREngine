#include "CRE_App.hpp"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
	CRE_App Application{};

	try
	{
		Application.Run();
	}
	catch (const std::exception& Exception)
	{
		std::cerr << Exception.what() << '\n';
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}