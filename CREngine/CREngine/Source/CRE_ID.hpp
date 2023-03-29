#pragma once

#include "CRE_Types.hpp"
#include "CRE_SimpleHashes.h"
#include "CRE_Utilities.hpp"

typedef uint32_t IDNum_t;

class CRE_ObjectIDRegistry
{
	friend class CRE_ID;
	friend class CRE_ObjectFactory;

	//Used for adding inital values to the map when making const IDs.
	static consteval void AddInitValue();

	static Map<IDNum_t, String>& GetMap();

	static CRE_ID CreateUniqueID(const String& In);
};

//Avoid implicit typecasting in this class.
class CRE_ID
{
	friend CRE_ObjectIDRegistry;
	IDNum_t Number = 0;
	uint32_t FlagValue = 0;
public:
	enum Flags : uint32_t
	{
		None = 0,
		HasBeenSet = 1 << 0,

		NUMBER_OFFSET = 1,
		Numbers = 0xFFFFFFFF << NUMBER_OFFSET,
	};
	//Sets the 31 bit aux number
	void SetFlags(const uint32_t& Value);
	uint32_t GetFlags() const;

	String GetString() const;
	IDNum_t GetNumber() const { return Number; }

	bool IsValidID() const;

	CRE_ID& operator = (const CRE_ID& CopyFrom);
	CRE_ID& operator = (const String& MakeFrom)
	{
		return *this = CRE_ID(MakeFrom);
	}
	bool operator == (const CRE_ID& CompareWith) const { return Number == CompareWith.Number; }

	CRE_ID(String Name);
	CRE_ID();

	//Helper for creating CRE_ID at compile time without ever having to do the crc
	template<StringLiteral LitString>
	static CRE_ID Constant()
	{
		constexpr auto Output = crc32_CONSTEVAL<LitString.Size>(LitString.Value);
		constexpr auto Str = LitString.Value;
		//Compiler only makes the ID once.
		static CRE_ID V = CRE_ID(Output, Str);
		return V;
	}

private:
	//Hidden constructor for statically created IDs using Constant
	CRE_ID(const IDNum_t& InNum, const String& InString);
};

//Implement std::hash for the object id so it can be used in maps.
namespace std
{
	template <>
	struct hash<CRE_ID>
	{
		std::size_t operator()(const CRE_ID& Item) const
		{
			auto out = Item.GetNumber();
			return out;
		}
	};
}

typedef CRE_ID ObjGUID;

template<>
struct Has_Serializer_Function<CRE_ID>
{
	constexpr static bool Value = true;
};

//manual serialize code for CRE_ID
static void VarSerialize(bool bSerializing, nlohmann::json& TargetJson, CRE_ID& Value)
{
	if (bSerializing)
	{
		TargetJson = Value.GetString();
	}
	else if (TargetJson.is_string())
	{
		Value = CRE_ID(TargetJson);
	}
}


