#pragma once

#include "CrTypes.hpp"
#include "CrSimpleHashes.h"
#include "CrUtilities.hpp"
#include "CrSerializationBin.h"

typedef uint32_t IDNum_t;
typedef uint8_t FlagSize;

class CrObjectIDRegistry
{
	friend class CrID;
	friend class CrObjectFactory;

	//Used for adding inital values to the map when making const IDs.
	static consteval void AddInitValue();

	static Map<IDNum_t, String>& GetMap();

	static CrID CreateUniqueID(const String& In);
};

//Avoid implicit typecasting in this class.
class CrID
{
	friend CrObjectIDRegistry;
	IDNum_t Number = 0;
	FlagSize FlagValue = 0;
public:
	enum Flags : FlagSize
	{
		None = 0,
		HasBeenSet = 1 << 0,

		NUMBER_OFFSET = 1,
		Numbers = 0xFF ^ NUMBER_OFFSET,
	};
	//Sets the 31 bit aux number
	void SetFlags(const FlagSize& Value);
	FlagSize GetFlags() const;

	String GetString() const;
	IDNum_t GetNumber() const { return Number; }

	bool IsValidID() const;

	CrID& operator = (const CrID& CopyFrom);
	CrID& operator = (const String& MakeFrom)
	{
		return *this = CrID(MakeFrom);
	}
	bool operator == (const CrID& CompareWith) const { return Number == CompareWith.Number; }

	CrID(String Name);
	CrID();

	//Helper for creating CrID at compile time without ever having to do the crc
	template<StringLiteral LitString>
	static CrID Constant()
	{
		constexpr auto Output = crc32_CONSTEVAL<LitString.Size>(LitString.Value);
		constexpr auto Str = LitString.Value;
		//Compiler only makes the ID once.
		static CrID V = CrID(Output, Str);
		return V;
	}

private:
	//Hidden constructor for statically created IDs using Constant
	CrID(const IDNum_t& InNum, const String& InString);
};

//Implement std::hash for the object id so it can be used in maps.
namespace std
{
	template <>
	struct hash<CrID>
	{
		std::size_t operator()(const CrID& Item) const
		{
			auto out = Item.GetNumber();
			return out;
		}
	};
}

typedef CrID ObjGUID;

template<>
struct Has_Serializer_Function<CrID>
{
	constexpr static bool Value = true;
};

//manual serialize code for CrID
static void VarSerialize(bool bSerializing, nlohmann::json& TargetJson, CrID& Value)
{
	if (bSerializing)
	{
		TargetJson = Value.GetString();
	}
	else if (TargetJson.is_string())
	{
		Value = CrID(TargetJson);
	}
}

//Special serialize for IDs.
template<>
inline static void operator <=><CrID>(CrArchive& Arch, CrID& ToSerialize)
{
	if (Arch.bSerializing)
	{
		Arch.save(ToSerialize.GetString());
	}
	else
	{
		std::string TempStr;
		Arch.load(TempStr);
		ToSerialize = CrID(std::move(TempStr));
	}
}

//A pair of CrIDs that can refer to a file on disk by being able to
//search back the path of the asset and the class for the extension/inital folder.
struct CrAssetReference
{
	//Should only ever be something like:
	//  /data/whateverAsset
	//  /shader/shaderName
	//  /assets/textureName
	//  /assets/objectName
	//This way, the ClassID can convert to the path plus extension
	CrID AssetID;

	//The class ref is a reference to the actual class name:
	//  CrManagedObject
	//  CrTexture
	//etc. This defines the extension for the file used when trying to load this asset.
	CrID ClassID;

	bool IsValidID() const 
	{
		return AssetID.IsValidID() && ClassID.IsValidID();
	}

	operator bool() const
	{
		return IsValidID();
	}
};

//Implement std::hash for the asset ref so it can be used in maps.
namespace std
{
	template <>
	struct hash<CrAssetReference>
	{
		std::size_t operator()(const CrAssetReference& Item) const
		{
			//Maybe find a better way of setting this up later
			//but for now since the CrID uses a 32bit number, and size_t is 64bit, just bitshift the class id and or them.
			std::size_t out = Item.AssetID.GetNumber();
			out |= ((std::size_t)Item.ClassID.GetNumber()) << 32;
			return out;
		}
	};
}

//Special serialize for IDs.
template<>
inline static void operator <=><CrAssetReference>(CrArchive& Arch, CrAssetReference& ToSerialize)
{
	Arch <=> ToSerialize.AssetID;
	Arch <=> ToSerialize.ClassID;
}
