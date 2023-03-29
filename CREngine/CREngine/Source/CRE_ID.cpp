#include "CRE_ID.hpp"
#include "CRE_SimpleHashes.h"

consteval void CRE_ObjectIDRegistry::AddInitValue()
{

}

Map<IDNum_t, String>& CRE_ObjectIDRegistry::GetMap()
{
	static Map<IDNum_t, String> Map { };
	return Map;
}

CRE_ID CRE_ObjectIDRegistry::CreateUniqueID(const String& In)
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

	CRE_ID Out;
	Out.FlagValue = true;
	Out.Number = UniqueValue;
	return Out;
}

void CRE_ID::SetFlags(const uint32_t& Value)
{
	FlagValue = Value << NUMBER_OFFSET;
}

uint32_t CRE_ID::GetFlags() const
{
	return FlagValue >> NUMBER_OFFSET;
}

String CRE_ID::GetString() const
{
	String Out = "";
	auto Map = CRE_ObjectIDRegistry::GetMap();
	auto Itr = Map.find(Number);
	if (Map.end() != Itr)
	{
		Out = Itr->second;
	}
	return Out;
}

bool CRE_ID::IsValidID() const
{
	return (FlagValue & HasBeenSet) && CRE_ObjectIDRegistry::GetMap().contains(Number);
}

CRE_ID& CRE_ID::operator=(const CRE_ID& CopyFrom)
{
	assert(CopyFrom.FlagValue);
	Number = CopyFrom.Number;
	FlagValue = CopyFrom.FlagValue;
	return *this;
}

CRE_ID::CRE_ID(String Name)
	: FlagValue(HasBeenSet)
{
	Number = crc32(Name.c_str(), Name.length());

	//Check if we have this number yet...
	Map<IDNum_t, String>& Map = CRE_ObjectIDRegistry::GetMap();
	if (!Map.contains(Number))
	{
		Map.emplace(Number, Name);
	}
}

CRE_ID::CRE_ID() : Number(0), FlagValue(0) { }

CRE_ID::CRE_ID(const IDNum_t& InNum, const String& InString) :
	Number(InNum),
	FlagValue(HasBeenSet)
{
	Map<IDNum_t, String>& Reg = CRE_ObjectIDRegistry::GetMap();
	//If this is hit, that means somehow this constructor was hit twice for the same key, which should NOT happen.
	assert(!Reg.contains(Number));
	Reg.emplace(Number, InString);
}
