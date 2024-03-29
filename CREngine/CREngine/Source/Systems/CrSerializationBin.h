#pragma once
#include <fstream>
#include <filesystem>
#include <assert.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include <memory>
#include "CrTypes.h"

#define USE_VARINT 0
#if USE_VARINT
#include "ThirdParty/ThirdPartyLibs.h"
#endif

#include "CrSimpleHashes.h"

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

class CrArchive
{
protected:
	std::fstream Stream;
	bool bIsNewFile;

	std::string BufferString;
	size_t BufferSize = 0;
	size_t ReadOffset = 0;

#if DEBUG_SERIALIZE
	//Number of serializes as a sanity check.
	uint64_t SCounter;
#endif

	//Minimum element size
	typedef uint64_t MinSize;
	CrArchive(const std::string& InPath)
	{
		if (std::filesystem::exists(InPath))
		{
			bIsNewFile = false;
		}
		else
		{
			bIsNewFile = true;
		}
	}
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
		//Handle shrinkage.
		Stream.close();
	}

	virtual void Setup(const std::string& InPath) { assert(0); }

	//Declare that the serialize function exists, but is specialized by the MemberFunction macro (inside the macro it specializes this function)
	template<typename PODType>
	void Serialize(PODType& InType) = delete;

	MemberFunction(CrArchive, bool);
	MemberFunction(CrArchive, char);//unfortunate side effect of char being hardware-specific.
	MemberFunction(CrArchive, wchar_t);//same as above
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

	//Stupid specialized wchar_t, since it doesn't interact well with a normal char fstream,
	//and I do not want to save into a wchar_t stream.
	template<typename Type, bool tSerializing>
	__forceinline void SerializeSpecific(Type& Item)
	{
		if constexpr (tSerializing)
		{
			Stream.write((char*) &Item, sizeof(Type));
		}
		else
		{
			Stream.read((char*) &Item, sizeof(Type));
		}
	}

	//returns true if it does need to reserialize
	template<bool tSerializing>
	__forceinline bool HashCheck(void* Ptr, size_t& SizeBytes)
	{
		if constexpr (tSerializing)
		{
			//Save the starting point.
			auto StartPoint = Stream.tellg();

			uint32_t BegHashNew = 0;
			uint32_t MidHashNew = 0;
			uint32_t EndHashNew = 0;

			bool bLargeEnough = false;

			if (SizeBytes >= 128)
			{
				bLargeEnough = true;
				BegHashNew = crc32((const char*)Ptr, 32);
				MidHashNew = crc32((const char*)Ptr + (SizeBytes / 2) - 16, 32);
				EndHashNew = crc32((const char*)Ptr + SizeBytes - 32, 32);
			}

			bool bIsSameHash = false;

			if (!bIsNewFile)
			{
				size_t OldSize = 0;
				uint32_t BegHashOld = 0;
				uint32_t MidHashOld = 0;
				uint32_t EndHashOld = 0;

				SerializeSpecific<size_t, false>(OldSize);
				SerializeSpecific<uint32_t, false>(BegHashOld);
				SerializeSpecific<uint32_t, false>(MidHashOld);
				SerializeSpecific<uint32_t, false>(EndHashOld);

				bIsSameHash =
					bLargeEnough &&
					OldSize == SizeBytes &&
					BegHashNew == BegHashOld &&
					MidHashNew == MidHashOld &&
					EndHashNew == EndHashOld;
			}

			if (!bIsSameHash)
			{
				//If it's not the same size/hash, write all values back into the file before we write our new data block.
				Stream.seekg(StartPoint);
				SerializeSpecific<size_t, true>(SizeBytes);
				SerializeSpecific<uint32_t, true>(BegHashNew);
				SerializeSpecific<uint32_t, true>(MidHashNew);
				SerializeSpecific<uint32_t, true>(EndHashNew);
				return true;
			}
		}
		else
		{
			uint32_t BegHashOld = 0;
			uint32_t MidHashOld = 0;
			uint32_t EndHashOld = 0;

			//Read the hashes/sizes in order to keep everything going in the correct order.
			SerializeSpecific<size_t, false>(SizeBytes);
			SerializeSpecific<uint32_t, false>(BegHashOld);
			SerializeSpecific<uint32_t, false>(MidHashOld);
			SerializeSpecific<uint32_t, false>(EndHashOld);

			//Maybe we could check the hashes here to ensure no alterations to the file were done...
		}
		return false;
	}

	//For large data sections, do a hash of the start, end, and midpoint ot check if it should be the same file.
	template<bool tSerializing>
	__forceinline void SerializeBytes(String& Array)
	{
		if constexpr (tSerializing)
		{
			size_t SizeBytes = Array.size();
			if (HashCheck<tSerializing>(Array.data(), SizeBytes))
			{
				Stream.write((char*)Array.data(), SizeBytes);
			}
		}
		else
		{
			size_t SizeBytes;
			HashCheck<tSerializing>(Array.data(), SizeBytes);
			Array.resize(SizeBytes);
			Stream.read((char*)Array.data(), SizeBytes);
		}
	}

	template<typename Type, bool tSerializing>
	__forceinline void SerializeSpecificContainer(Type& Item)
	{
		//retrieve element type that the container uses.
		typedef std::type_identity<std::remove_cv_t<typename Type::value_type>>::type ElemType;

		uint64_t V = 0;
		if constexpr (tSerializing)
		{
			V = Item.size();
		}
		//Serialize/deserialize size first so we know how long to loop.
		SerializeSpecific<uint64_t, tSerializing>(V);
		if constexpr (!tSerializing)
		{
			Item.resize(V);
		}

		//If it's a POD/fundamental type we can just copy the contents of the container in a strip.
		if constexpr (std::is_fundamental<ElemType>::value)
		{
			if constexpr (tSerializing)
			{
				Stream.write((char*)&Item[0], V * sizeof(ElemType));
			}
			else
			{
				Stream.read((char*)&Item[0], V * sizeof(ElemType));
			}
		}
		else if constexpr (std::is_fundamental<ElemType>::value == false)
		{
			constexpr bool HasSerFunc = requires(CrArchive & Ar, ElemType & Itm) { Ar.SerializeSpecific<ElemType, tSerializing>(Itm); };
			constexpr bool HasSerOper = requires(CrArchive & Ar, ElemType & Itm) { Ar <=> Itm; };
			//Must have some type of serialization setup for these.
			static_assert(HasSerFunc == true || HasSerOper == true);

			for (uint64_t i = 0; i < V; i++)
			{
				if constexpr (HasSerFunc)
				{
					//More optimized since it goes right to the serialize specific function instead of calling the func ptr
					SerializeSpecific<ElemType, tSerializing>(Item[i]);
				}
				else if constexpr (HasSerOper)
				{
					*this <=> Item[i];
				}
			}
		}
	}
};

template<bool ShouldSerialize>
class CrArchive_Implement : public CrArchive
{
public:
	CrArchive_Implement(const std::string& InPath) : CrArchive(InPath)
	{
		bSerializing = ShouldSerialize;

		constexpr std::ios::openmode OpenMode = std::ios::binary | (ShouldSerialize ? std::ios::out : std::ios::in);
		Stream = std::fstream{ InPath, OpenMode };
		if (!Stream.is_open())
		{
			assert(0);
		}

		VariableFunction(CrArchive, bool) = &CrArchive::SerializeSpecific<bool, ShouldSerialize>;
		VariableFunction(CrArchive, char) = &CrArchive::SerializeSpecific<char, ShouldSerialize>;
		VariableFunction(CrArchive, wchar_t) = &CrArchive::SerializeSpecific<wchar_t, ShouldSerialize>;
		VariableFunction(CrArchive, uint8_t) = &CrArchive::SerializeSpecific<uint8_t, ShouldSerialize>;
		VariableFunction(CrArchive, uint16_t) = &CrArchive::SerializeSpecific<uint16_t, ShouldSerialize>;
		VariableFunction(CrArchive, uint32_t) = &CrArchive::SerializeSpecific<uint32_t, ShouldSerialize>;
		VariableFunction(CrArchive, uint64_t) = &CrArchive::SerializeSpecific<uint64_t, ShouldSerialize>;
		VariableFunction(CrArchive, int8_t) = &CrArchive::SerializeSpecific<int8_t, ShouldSerialize>;
		VariableFunction(CrArchive, int16_t) = &CrArchive::SerializeSpecific<int16_t, ShouldSerialize>;
		VariableFunction(CrArchive, int32_t) = &CrArchive::SerializeSpecific<int32_t, ShouldSerialize>;
		VariableFunction(CrArchive, int64_t) = &CrArchive::SerializeSpecific<int64_t, ShouldSerialize>;
		VariableFunction(CrArchive, float) = &CrArchive::SerializeSpecific<float, ShouldSerialize>;
		VariableFunction(CrArchive, double) = &CrArchive::SerializeSpecific<double, ShouldSerialize>;

		//String types.
		VariableFunction(CrArchive, String) = &CrArchive::SerializeSpecificContainer<String, ShouldSerialize>;
		VariableFunction(CrArchive, WString) = &CrArchive::SerializeSpecificContainer<WString, ShouldSerialize>;
	}
};

//Archive type that loads a file.
typedef CrArchive_Implement<false> CrArchiveIn;

//Archive that serializes to a file.
typedef CrArchive_Implement<true> CrArchiveOut;

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

//For basic resizable containers, currently only vector (although it uses the same code as string and wstring)
template<typename T>
__forceinline static void operator <=>(CrArchive& Arch, T& ToSerialize) requires is_container<T>::value
{
	if (Arch.bSerializing)
	{
		Arch.SerializeSpecificContainer<T, true>(ToSerialize);
	}
	else
	{
		Arch.SerializeSpecificContainer<T, false>(ToSerialize);
	}
}

//For unordered/hashed containers. - unordered_map and unordered_set
//Maybe I can come up with a better implementation in the future.
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

//char size binary array - literally just a string with a derived type in order to allow separate serialization type.
//This is clarified to be BinArray so it is clear that this is not a displayable string.
struct BinArray : public String
{
	
};

using BinArrayV = StringV;

//Specialization for binary array
__forceinline static void operator <=>(CrArchive& Arch, BinArray& ToSerialize)
{
	//Serialize all the binary data
	if (ToSerialize.size() && Arch.bSerializing)
	{
		Arch.SerializeBytes<true>(ToSerialize);
	}
	else if (!Arch.bSerializing)
	{
		Arch.SerializeBytes<false>(ToSerialize);
	}
}
