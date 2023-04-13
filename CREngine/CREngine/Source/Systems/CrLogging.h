#pragma once
#include <iostream>

//We should NOT use printf ever, it isn't as powerful or safe as std::format.

//References for format arguments and such:
// https://en.cppreference.com/w/cpp/utility/format/format
//This one shows how you can adjust the output of certain variable types:
// https://en.cppreference.com/w/cpp/utility/format/formatter

//Version that uses format instead of printf.
#define CrLOG(Text, ...) std::cout << std::format(##Text, __VA_ARGS__) << std::endl

//Activatable log version

#define CrLOGD(MacroDef, Text, ...)\
if constexpr (MacroDef)\
	std::cout << std::format(##Text, __VA_ARGS__) << std::endl

//Activatable log version

#define CrCLOGD(Condition, MacroDef, Text, ...)									\
if constexpr (MacroDef)															\
{																				\
	if (Condition)																\
	{																			\
		std::cout << std::format(##Text, __VA_ARGS__) << std::endl;				\
	}																			\
}
