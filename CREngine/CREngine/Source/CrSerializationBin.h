#pragma once
#include <fstream>
#include <filesystem>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <memory>
#include "CrTypes.hpp"

#define USE_VARINT 0
#if USE_VARINT
#include "ThirdParty/ThirdPartyLibs.h"
#endif

constexpr uint64_t Zero64 = 0x00000000;

#define MemberFunction(ThisClass, Type) void (##ThisClass::*Serialize_##Type)(Type&);	 \
template<>																				 \
__forceinline void Serialize<Type>(Type& Value) { (this->*Serialize_##Type)(Value); }		

#define DefSerializeFunc(Type, Name, IO) void Serialize_##Type_##IO(bool& ##Name)

#define VariableFunction(ThisClass, Type) this->Serialize_##Type

#define DEBUG_SERIALIZE 0
#if DEBUG_SERIALIZE
//Remove later.
#include "CrLogging.h"
#endif

//class CrArchive;
//template<class T> using Has_Serialize_Function_t = decltype(std::declval<CrArchive>().Serialize(std::declval<T&>()));
//template<class T> using Has_Serialize_Operator_t = decltype(std::declval<CrArchive&>() <=> std::declval<T&>());
//
//template<typename T> constexpr bool Has_Serialize_Function = std::is_detected_v<Has_Serialize_Function_t, T>;
//template<typename T> constexpr bool Has_Serialize_Operator = std::is_detected_v<Has_Serialize_Operator_t, T>;

class CrArchive
{
protected:
	std::fstream Stream;
	
	bool bEncode;
	std::string BufferString;
	size_t BufferSize = 0;
	size_t ReadOffset = 0;

#if DEBUG_SERIALIZE
	//Number of serializes as a sanity check.
	uint64_t SCounter;
#endif

	//Minimum element size
	typedef uint64_t MinSize;
	CrArchive() {}
public:
	bool bSerializing;

	~CrArchive()
	{
#if DEBUG_SERIALIZE
		if (bSerializing)
		{
			Stream << SCounter;
		}
		else
		{
			uint64_t SavedSCounter = 0;
			Stream >> SavedSCounter;
			CrLOG("SavedSCounter at: %u      x", SavedSCounter);
		}
		CrLOG("SCounter at: %u      x", SCounter);
#endif
		Stream.close();
	}

	virtual void Setup(const std::string& InPath) { assert(0); }

	template<typename PODType>
	void Serialize(PODType& InType)
	{
		//Unhandled type tried to be serialized - please implement that type.
		assert(0);
	}

	//This is really the core of functionality in the whole class - it provides a specialization toward serialization or deserialization
	template<typename Type, bool bSerializing, bool Encoding> 
	__forceinline void SerializeSpecific(Type& Item)
	{
#if DEBUG_SERIALIZE
		SCounter++;
#endif
#if USE_VARINT
		if constexpr (Encoding == false)
#endif
		{
			//Write/read directly to/from the stream if not encoding/compressing the values.
			if constexpr (bSerializing)
			{
				Stream << Item;
			}
			else
			{
				Stream >> Item;
			}
		}
		//Disabled for the moment until I have time to figure out compression and such.
#if USE_VARINT
		else
		{
			constexpr std::size_t TypeSize = sizeof(Type);
			if constexpr (bSerializing)
			{
				if constexpr (std::is_integral_v<Type> && TypeSize > 2)
				{
					BufferSize += tser::encode_varint(Item, BufferString.data() + BufferSize);
				}
				else
				{
					//currently crashes here - but I don't care enough to fix it for the moment.
					std::memcpy(BufferString.data() + BufferSize, std::addressof(Item), TypeSize);
					BufferSize += TypeSize;
				}
			}
			else
			{
				if constexpr (std::is_integral_v<Type> && TypeSize > 2)
				{
					ReadOffset += tser::decode_varint(Item, BufferString.data() + ReadOffset);
				}
				else 
				{
					std::memcpy(&Item, BufferString.data() + ReadOffset, TypeSize);
					ReadOffset += TypeSize;
				}
			}
		}
#endif
	}


	template<typename Type, bool bSerializing, bool Encoding>
	__forceinline void SerializeSpecificContainer(Type& Item)
	{
		uint64_t V = 0;
		if constexpr (bSerializing)
		{
			V = Item.size();
		}
		//Serialize/deserialize size first so we know how long to loop.
		Serialize(V);
		if constexpr (!bSerializing)
		{
			Item.resize(V);
		}

		constexpr bool HasSerFunc = requires(CrArchive& Ar, Type::value_type& Itm) { Ar.Serialize(Itm); };
		constexpr bool HasSerOper = requires(CrArchive& Ar, Type::value_type& Itm) { Ar <=> Itm; };

		for (uint64_t i = 0; i < V; i++)
		{
			if constexpr (HasSerFunc)
			{
				Serialize(Item[i]);
			}
			else if constexpr (HasSerOper)
			{
				*this <=> Item[i];
			}
			else
			{
				assert(0);//?????
			}
		}
	}

	MemberFunction(CrArchive, bool);
	MemberFunction(CrArchive, char);//unfortunate side effect of char being hardware-specific.
	MemberFunction(CrArchive, uint8_t);
	MemberFunction(CrArchive, uint16_t);
	MemberFunction(CrArchive, uint32_t);
	MemberFunction(CrArchive, uint64_t);
	MemberFunction(CrArchive, int8_t);
	MemberFunction(CrArchive, int16_t);
	MemberFunction(CrArchive, int32_t);
	MemberFunction(CrArchive, int64_t);
	MemberFunction(CrArchive, float);
	MemberFunction(CrArchive, double);

	//String types for simplicity since they're pretty straightforward since their value types are known here (which arrays do not have the advantage of)
	MemberFunction(CrArchive, String);
	MemberFunction(CrArchive, WString);
};

template<bool ShouldSerialize, bool bEncoding>
class CrArchive_Implement : public CrArchive
{
public:
	CrArchive_Implement(const std::string& InPath)
	{
		bSerializing = ShouldSerialize;

		constexpr std::ios::openmode OpenMode = std::ios::binary | (ShouldSerialize ? std::ios::out : std::ios::in);
		Stream = std::fstream{ InPath, OpenMode };
		if (!Stream.is_open())
		{
			assert(0);
		}

		if constexpr (bEncoding && !ShouldSerialize)
		{
			//Load the whole value (big copy potentially).
			Stream >> BufferString;
		}

		VariableFunction(CrArchive, bool) = &CrArchive::SerializeSpecific<bool, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, char) = &CrArchive::SerializeSpecific<char, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, uint8_t) = &CrArchive::SerializeSpecific<uint8_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, uint16_t) = &CrArchive::SerializeSpecific<uint16_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, uint32_t) = &CrArchive::SerializeSpecific<uint32_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, uint64_t) = &CrArchive::SerializeSpecific<uint64_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, int8_t) = &CrArchive::SerializeSpecific<int8_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, int16_t) = &CrArchive::SerializeSpecific<int16_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, int32_t) = &CrArchive::SerializeSpecific<int32_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, int64_t) = &CrArchive::SerializeSpecific<int64_t, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, float) = &CrArchive::SerializeSpecific<float, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, double) = &CrArchive::SerializeSpecific<double, ShouldSerialize, bEncoding>;

		//String types.
		VariableFunction(CrArchive, String) = &CrArchive::SerializeSpecificContainer<String, ShouldSerialize, bEncoding>;
		VariableFunction(CrArchive, WString) = &CrArchive::SerializeSpecificContainer<WString, ShouldSerialize, bEncoding>;
	}

	~CrArchive_Implement()
	{
		if constexpr (bEncoding && ShouldSerialize)
		{
			//Save the whole value (big copy potentially).
			Stream << BufferString;
		}
	}
};

//Archive type that loads a file.
typedef CrArchive_Implement<false, USE_VARINT> CrArchiveIn;

//Archive that serializes to a file.
typedef CrArchive_Implement<true, USE_VARINT> CrArchiveOut;

struct CrBinSerializable
{
	virtual void BinSerialize(CrArchive& Data) = 0;
};

//Container type traits
template<class T> struct is_container : public std::false_type {};
template<class T, class Alloc> struct is_container<std::vector<T, Alloc>> : public std::true_type {};
//template<> struct is_container<std::string> : public std::true_type {};
//template<> struct is_container<std::wstring> : public std::true_type {};

//Map/set type traits
template<class T> struct is_hashed_container : public std::false_type {};
template<class K, class T, class Hash, class Comp, class Alloc>
struct is_hashed_container<std::unordered_map<K, T, Hash, Comp, Alloc>> : public std::true_type {};
template<class K, class Hash, class Comp, class Alloc>
struct is_hashed_container<std::unordered_set<K, Hash, Comp, Alloc>> : public std::true_type {};

//use spaceship operator because I want to. Implement this to allow serialization of special classes.
template<typename T>
__forceinline static void operator <=>(CrArchive& Arch, T& ToSerialize)
{
	//Serialize complete PODs.
	Arch.Serialize(ToSerialize);
}

//Spec for std::pair.
template<typename Key, typename Value>
__forceinline static void operator <=>(CrArchive& Arch, std::pair<Key, Value> & ToSerialize)
{
	Arch <=> ToSerialize->first;
	Arch <=> ToSerialize->second;
}

//For basic resizable containers, currently only vector.
template<typename T>
__forceinline static void operator <=>(CrArchive& Arch, T& ToSerialize) requires is_container<T>::value
{
	uint64_t V = 0;
	if (Arch.bSerializing)
	{
		V = ToSerialize.size();
		Arch.Serialize(V);
	}
	else
	{
		Arch.Serialize(V);
		ToSerialize.resize(V);
	}

	for (uint64_t i = 0; i < V; i++)
	{
		Arch <=> ToSerialize[i];
	}
}

//For unordered/hashed containers. - unordered_map and unordered_set
template<typename T>
__forceinline static void operator <=>(CrArchive& Arch, T& ToSerialize) requires is_hashed_container<T>::value
{
	uint64_t Size = 0;
	if (Arch.bSerializing)
	{
		Size = ToSerialize.size();
		Arch.Serialize(Size);

		for (typename T::value_type& Elem : ToSerialize)
		{
			Arch <=> Elem;
		}
	}
	else
	{
		Arch.Serialize(Size);

		for (uint64_t i = 0; i < Size; i++)
		{
			typename T::value_type TempVType;
			Arch <=> TempVType;
			ToSerialize.emplace(TempVType);
		}
	}
}

//Specialization for paths - really just uses wstring serialization in reality.
__forceinline static void operator <=>(CrArchive& Arch, std::filesystem::path& ToSerialize)
{
	//In between
	std::wstring WString;

	if (Arch.bSerializing)
	{
		WString = ToSerialize.c_str();
	}

	Arch <=> WString;

	if (!Arch.bSerializing)
	{
		ToSerialize = WString;
	}
}
