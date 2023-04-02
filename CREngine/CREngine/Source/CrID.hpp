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

	static Map<IDNum_t, uint64_t>& GetMap();

	//We keep the strings separated from the map since we need stable string locations for making std::string_views into them.
	static Array<String>& GetStringArr();

	static void Emplace(IDNum_t Number, String&& InString);

	static CrID CreateUniqueID(const String& In);

	template<bool bPretty = false>
	forceinline static StringV GetStringImpl(const IDNum_t& Number)
	{
		auto Map = CrObjectIDRegistry::GetMap();
		auto Itr = Map.find(Number);
		if (Map.end() != Itr)
		{
			if constexpr (bPretty)
			{
				//Shorten string view
				StringV View = StringV(GetStringArr()[Itr->second]);
				return View.substr(0, View.find_last_of('_'));
			}
			else
			{
				return StringV(GetStringArr()[Itr->second]);
			}
		}
		return StringV("");
	}
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

	StringV GetString() const;
	StringV GetStringPretty() const;
	IDNum_t GetNumber() const { return Number; }

	bool IsValidID() const;

	CrID& operator = (const CrID& CopyFrom);
	CrID& operator = (const String& MakeFrom)
	{
		return *this = CrID(MakeFrom);
	}
	bool operator == (const CrID& CompareWith) const { return Number == CompareWith.Number; }

	CrID(const String& Name);
	CrID(String&& Name);
	CrID();

	//Helper for creating CrID at compile time without ever having to do the crc
	template<StringLiteral LitString>
	static CrID Constant()
	{
		constexpr uint32_t Output = crc32_CONSTEVAL<LitString.Size>(LitString.Value);
		constexpr auto Str = LitString.Value;

		//CrLOG("Static ID Made:   <%ul>    <%s> sz:<%ull>     x", Output, LitString.Value, LitString.Size);

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
inline static void operator <=>(CrArchive& Arch, CrID& ToSerialize)
{
	if (Arch.bSerializing)
	{
		//Must copy since it's a string_view
		String TempStr = String(ToSerialize.GetString());
		Arch <=> TempStr;
	}
	else
	{
		String TempStr;
		Arch <=> TempStr;
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

	bool operator == (const CrAssetReference& Other) const
	{
		return AssetID == Other.AssetID && ClassID == Other.ClassID;
	}

	String GetString() const;

	explicit CrAssetReference(const CrID& InAssetID, const CrID& InClassID) : AssetID(InAssetID), ClassID(InClassID) {}
	explicit CrAssetReference() : AssetID(), ClassID() {}
	CrAssetReference(const CrID&) = delete; // delete this constructor since it can have unintended side effects.
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
inline static void operator <=>(CrArchive& Arch, CrAssetReference& ToSerialize)
{
	Arch <=> ToSerialize.AssetID;
	Arch <=> ToSerialize.ClassID;
}
