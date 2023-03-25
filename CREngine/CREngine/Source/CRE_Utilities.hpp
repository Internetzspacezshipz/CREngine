#pragma once

#include "CRE_Types.hpp"

template<typename VecElementType, typename PredicateType>
forceinline void RemoveByPredicate(Array<VecElementType>& Vector, const PredicateType& Predicate)
{
	if (Vector.size())
	{
		Vector.erase(std::remove_if(Vector.begin(), Vector.end(), Predicate), Vector.end());
	}
}

template<typename KeyType, typename ElementType, typename PredicateType>
forceinline void RemoveByPredicate(Map<KeyType, ElementType>& Map, const PredicateType& Predicate)
{
	if (Map.size())
	{
		Map.erase(std::remove_if(Map.begin(), Map.end(), Predicate), Map.end());
	}
}

template<typename KeyType, typename PredicateType>
forceinline void RemoveByPredicate(Set<KeyType>& Set, const PredicateType& Predicate)
{
	if (Set.size())
	{
		Set.erase(std::remove_if(Set.begin(), Set.end(), Predicate), Set.end());
	}
}

#define CARRAYSIZE(_ARR) ((int)(sizeof(_ARR) / sizeof(*(_ARR))))
