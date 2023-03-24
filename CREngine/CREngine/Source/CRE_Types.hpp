#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <ThirdParty/func/func.hpp>


template<typename ... args>
using Array = std::vector<args...>;

template<typename ... args>
using Map = std::unordered_map<args...>;

template<typename ... args>
using Set = std::unordered_set<args...>;

template<typename ... args>
using Func = fu2::function<args...>;

