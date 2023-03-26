#pragma once

#include "CRE_Types.hpp"

template<typename VecElementType, typename PredicateType>
forceinline void RemoveByPredicate(Array<VecElementType>& Target, const PredicateType& Predicate)
{
	if (Target.size())
	{
		Target.erase(std::remove_if(Target.begin(), Target.end(), Predicate), Target.end());
	}
}

template<typename KeyType, typename ElementType, typename PredicateType>
forceinline void RemoveByPredicate(Map<KeyType, ElementType>& Target, const PredicateType& Predicate)
{
	for (auto It = Target.begin(); It != Target.end();)
	{
		if (Predicate(*It))
		{
			It = Target.erase(It);
		}
		else
		{
			It++;
		}
	}
}

template<typename KeyType, typename PredicateType>
forceinline void RemoveByPredicate(Set<KeyType>& Target, const PredicateType& Predicate)
{
	if (Target.size())
	{
		Target.erase(std::remove_if(Target.begin(), Target.end(), Predicate), Target.end());
	}
}

#define CARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR))))

//for consteval of string length
template <std::size_t S>
consteval std::size_t strlen_consteval(char const (&)[S]) { return S - 1; }

//template argument usable string
template<size_t N>
struct StringLiteral
{
	consteval StringLiteral(const char(&Str)[N])
	{
		std::copy_n(Str, N, Value);
	}

	std::size_t Size = N - 1;
	char Value[N];
};

