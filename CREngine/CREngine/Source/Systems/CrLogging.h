#pragma once
#include <iostream>

//Uses printf
#define CrLOG(Text, ...) std::cout << std::printf(##Text, __VA_ARGS__) << std::endl

//Version that uses format instead of printf.
#define CrLOGf(Text, ...) std::cout << std::format(##Text, __VA_ARGS__) << std::endl

//Activatable log version
#define CrLOGD(MacroDef, Text, ...)\
if constexpr (MacroDef)\
	std::cout << std::printf(##Text, __VA_ARGS__) << std::endl

#define CrLOGDf(MacroDef, Text, ...)\
if constexpr (MacroDef)\
	std::cout << std::format(##Text, __VA_ARGS__) << std::endl

//Activatable log version
#define CrCLOGD(Condition, MacroDef, Text, ...)									\
if constexpr (MacroDef)															\
{																				\
	if (Condition)																\
	{																			\
		std::cout << std::printf(##Text, __VA_ARGS__) << std::endl;				\
	}																			\
}

#define CrCLOGDf(Condition, MacroDef, Text, ...)								\
if constexpr (MacroDef)															\
{																				\
	if (Condition)																\
	{																			\
		std::cout << std::format(##Text, __VA_ARGS__) << std::endl;				\
	}																			\
}
