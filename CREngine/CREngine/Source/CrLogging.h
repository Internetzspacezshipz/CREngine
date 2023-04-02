#pragma once
#include <iostream>

#define CrLOG(Text, ...) std::cout << std::printf(##Text, __VA_ARGS__) << std::endl

//Activatable log version
#define CrLOGD(MacroDef, Text, ...)\
if constexpr (MacroDef)\
	std::cout << std::printf(##Text, __VA_ARGS__) << std::endl
