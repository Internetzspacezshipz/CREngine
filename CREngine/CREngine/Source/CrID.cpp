#include "CrID.hpp"
#include "CrSimpleHashes.h"

consteval void CrObjectIDRegistry::AddInitValue()
{

}

Map<IDNum_t, String>& CrObjectIDRegistry::GetMap()
{
	static Map<IDNum_t, String> Map { };
	return Map;
}

CrID CrObjectIDRegistry::CreateUniqueID(const String& In)
{
	Map<IDNum_t, String>& Map = GetMap();
	int Index = 0;
	String UniqueString = In;
	IDNum_t UniqueValue = 0;
	do
	{
		UniqueString = In + std::format("_{}", Index++);
		UniqueValue = crc32(UniqueString);
	} while (Map.find(UniqueValue) != Map.end());

	Map.emplace(UniqueValue, UniqueString);

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

String CrID::GetString() const
{
	String Out = "";
	auto Map = CrObjectIDRegistry::GetMap();
	auto Itr = Map.find(Number);
	if (Map.end() != Itr)
	{
		Out = Itr->second;
	}
	return Out;
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
	Map<IDNum_t, String>& Map = CrObjectIDRegistry::GetMap();
	if (!Map.contains(Number))
	{
		Map.emplace(Number, std::move(Name));
	}
}

CrID::CrID() : Number(0), FlagValue(0) { }

CrID::CrID(const IDNum_t& InNum, const String& InString) :
	Number(InNum),
	FlagValue(HasBeenSet)
{
	Map<IDNum_t, String>& Reg = CrObjectIDRegistry::GetMap();
	//If this is hit, that means somehow this constructor was hit twice for the same key, which should NOT happen.
	assert(!Reg.contains(Number));
	Reg.emplace(Number, InString);
}
