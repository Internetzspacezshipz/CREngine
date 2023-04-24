#pragma once
#include <iostream>
#include <format>
#include <source_location>

//We should NOT use printf ever, it isn't as powerful or safe as std::format.

//References for format arguments and such:
// https://en.cppreference.com/w/cpp/utility/format/format
//This one shows how you can adjust the output of certain variable types:
// https://en.cppreference.com/w/cpp/utility/format/formatter

static const char* FileNameOnly(const char* str)
{
	if (str == nullptr)
	{
		return "None";
	}

	const char* last = nullptr;
	for (size_t i = 0; str[i] != '\0'; ++i)
	{
		if (str[i] == '\\')
		{
			last = &str[i];
		}
	}
	return last ? last + 1 : str;
}

//Version that uses format instead of printf.
#define CrLOG(Text, ...) std::cout << std::format("{}:{}:{} - {}", FileNameOnly(std::source_location::current().file_name()), std::source_location::current().function_name(), std::source_location::current().line(), std::format(##Text, __VA_ARGS__)) << std::endl

//Activatable log version

#define CrLOGD(MacroDef, Text, ...)\
if constexpr (MacroDef)\
	std::cout << std::format("{}:{}:{} - {}", FileNameOnly(std::source_location::current().file_name()), std::source_location::current().function_name(), std::source_location::current().line(), std::format(##Text, __VA_ARGS__)) << std::endl

//Activatable log version
#define CrCLOGD(Condition, MacroDef, Text, ...)																					\
if constexpr (MacroDef)																											\
{																																\
	if (Condition)																												\
	{																															\
		std::cout << std::format("{}:{}:{} - {}", FileNameOnly(std::source_location::current().file_name()), std::source_location::current().function_name(), std::source_location::current().line(), std::format(##Text, __VA_ARGS__)) << std::endl;				\
	}																															\
}
