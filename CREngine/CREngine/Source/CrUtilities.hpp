#pragma once

#include "CrTypes.hpp"

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

template<typename EnumT>
struct FlagCombiner
{
	template<EnumT ...>                                                 // (1)
	struct Combine;

	template<>                                                        // (2)
	struct Combine<>
	{
		static const EnumT V = (EnumT)0;
	};

	template<EnumT in, EnumT ... args>                                     // (3)
	struct Combine<in, args ...>
	{
		//Dumb amount of casting here.
		static const EnumT V = (EnumT)(((uint64_t)in) | ((uint64_t)Combine<args ...>::V));
	};
};

