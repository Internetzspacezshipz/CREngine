#include "CrID.hpp"
#include "CrSimpleHashes.h"
#include "CrSerialization.hpp"


consteval void CrObjectIDRegistry::AddInitValue()
{

}

Map<IDNum_t, uint32_t>& CrObjectIDRegistry::GetMap()
{
	static Map<IDNum_t, uint32_t> Map { };
	return Map;
}

Array<String>& CrObjectIDRegistry::GetStringArr()
{
	static Array<String> Arr{ };
	return Arr;
}

void CrObjectIDRegistry::Emplace(IDNum_t Number, String&& InString)
{
	Array<String>& Arr = GetStringArr();
	auto V = Arr.size();
	Arr.push_back(std::move(InString));
	GetMap().emplace(Number, V);
}

CrID CrObjectIDRegistry::CreateUniqueID(const String& In)
{
	Map<IDNum_t, uint32_t>& Map = GetMap();
	Array<String>& StrArr = GetStringArr();
	int Index = 0;
	String UniqueString = In;
	IDNum_t UniqueValue = 0;
	do
	{
		UniqueString = In + std::format("_{}", Index++);
		UniqueValue = crc32(UniqueString);
	} while (Map.find(UniqueValue) != Map.end());

	Emplace(UniqueValue, std::move(UniqueString));

	CrID Out;
	Out.FlagValue = true;
	Out.Number = UniqueValue;
	return Out;
}

void CrID::SetFlags(const FlagSize& Value)
{
	FlagValue = Value << NUMBER_OFFSET;
}

FlagSize CrID::GetFlags() const
{
	return FlagValue >> NUMBER_OFFSET;
}


StringV CrID::GetString() const
{
	return CrObjectIDRegistry::GetStringImpl<false>(Number);
}

StringV CrID::GetStringPretty() const
{
	return CrObjectIDRegistry::GetStringImpl<true>(Number);
}

bool CrID::IsValidID() const
{
	return (FlagValue & HasBeenSet) && CrObjectIDRegistry::GetMap().contains(Number);
}

CrID& CrID::operator=(const CrID& CopyFrom)
{
	assert(CopyFrom.FlagValue);
	Number = CopyFrom.Number;
	FlagValue = CopyFrom.FlagValue;
	return *this;
}

CrID::CrID(String Name)
	: FlagValue(HasBeenSet)
{
	Number = crc32(Name.c_str(), Name.length());

	//Check if we have this number yet...
	auto& Map = CrObjectIDRegistry::GetMap();
	if (!Map.contains(Number))
	{
		CrObjectIDRegistry::Emplace(Number, std::move(Name));
	}
}

CrID::CrID() : Number(0), FlagValue(0) { }

CrID::CrID(const IDNum_t& InNum, const String& InString) :
	Number(InNum),
	FlagValue(HasBeenSet)
{
	auto& Reg = CrObjectIDRegistry::GetMap();
	//If this is hit, that means somehow this constructor was hit twice for the same key, which should NOT happen.
	assert(!Reg.contains(Number));
	CrObjectIDRegistry::Emplace(Number, String(InString));
}

String CrAssetReference::GetString() const
{
	String OutString = String(AssetID.GetString());
	OutString.append(CrSerialization::Get().GetExtensionForClass(ClassID));
	return OutString;
}

