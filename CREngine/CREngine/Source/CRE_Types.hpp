#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ThirdParty/func/func.hpp>
#include <filesystem>
#include <json.hpp>


template<typename ... args>
using Array = std::vector<args...>;

template<typename ... args>
using Map = std::unordered_map<args...>;

template<typename ... args>
using Set = std::unordered_set<args...>;

template<typename ... args>
using Func = fu2::function<args...>;

using Path = std::filesystem::path;

using String = std::string;

using Json = nlohmann::json;

//Serialiation function type trait decl
template<typename Type>
struct Has_Serializer_Function
{
	constexpr static bool Value = false;
};

//def for forceinline to make it easier to type and also rename later if needed.
#define forceinline __forceinline

//hide shared_ptr and such.
#define SP std::shared_ptr
#define WP std::weak_ptr
#define UP std::unique_ptr

#define Pair std::pair

//make shared helper
#define MkSP std::make_shared
#define MkUP std::make_unique
