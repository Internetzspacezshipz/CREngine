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
	template<EnumT ...>                                           
	struct Combine;

	template<>                                                    
	struct Combine<>
	{
		static const EnumT V = (EnumT)0;
	};

	template<EnumT in, EnumT ... args>                            
	struct Combine<in, args ...>
	{
		//Dumb amount of casting here.
		static const EnumT V = (EnumT)(((uint64_t)in) | ((uint64_t)Combine<args ...>::V));
	};
};


template<auto K, auto V>
struct SPair
{
	constexpr static auto Key = K;
	constexpr static auto Val = V;
};

template<auto K, auto ...V>
struct SPairT
{
	constexpr static auto Key = K;
	constexpr static auto Val = Tuple{ V... };
};


template<typename... Items>
struct Chain;

template<>
struct Chain<>
{
	constexpr static bool Valid = false;
	constexpr static auto Get(auto TestKey)
	{
		return 0;
	}
};

//??????
template<typename TItem, typename... Next>
struct Chain<TItem, Next...>
{
	using Item = TItem;
	constexpr static bool Valid = true;
	consteval static auto Get_consteval(auto TestKey)
	{
		return Get(TestKey);
	}

	constexpr static auto Get(auto TestKey)
	{
		if (Item::Key == TestKey)
		{
			return Item::Val;
		}
		else if constexpr (Chain<Next...>::Valid)
		{
			return Chain<Next...>::Get(TestKey);
		}
	}
	
	template<typename Visitor>
	static void Visit(const Visitor& Vis)// requires std::is_same_v<decltype(&Visitor::operator()), void(std::add_const_t<decltype(Item::Key)>, std::add_const_t<decltype(Item::Val)>)>
	{
		if constexpr (Chain<Next...>::Valid)
		{
			Vis(Item::Key, Item::Val);
			Chain<Next...>::Visit(Vis);
		}
		else //for last one.
		{
			Vis(Item::Key, Item::Val);
		}
	}
};

#define EnumText(EnumValue) SPair<EnumValue, StringLiteral2(""#EnumValue"")>

//Compile time map that you can use to map out items related to each other without having to make a whole map and everything.
/*
enum SomeEnum : uint8_t
{
	One, Two, Three
};

typedef Chain<
	SPair<One, 6>, 
	SPair<Two, 171>, 
	SPair<Three, 51>, 
	SPair<61, 555>> Something;

constexpr int v1 = Something::Get(One);
constexpr int v2 = Something::Get(Two);
constexpr int v3 = Something::Get(Three);
constexpr int v4 = Something::Get(61);
*/