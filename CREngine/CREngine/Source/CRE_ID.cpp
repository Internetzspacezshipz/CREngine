#include "CRE_ID.hpp"
#include "CRE_SimpleHashes.h"

Map<IDNum_t, String>& CRE_ObjectIDRegistry::GetMap()
{
	static Map<IDNum_t, String> Map;
	return Map;
}

String CRE_ObjectIDRegistry::CreateUniqueString(const String& In)
{
	Map<IDNum_t, String>& Map = GetMap();
	int Index = 0;
	String UniqueString = In;
	do
	{
		UniqueString = In + std::format("_{}", Index++);
	} while (Map.find(crc32(UniqueString)) != Map.end());
	return UniqueString;
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
	return bHasBeenSet && CRE_ObjectIDRegistry::GetMap().contains(Number);
}

CRE_ID& CRE_ID::operator=(const CRE_ID& CopyFrom)
{
	assert(CopyFrom.bHasBeenSet);
	Number = CopyFrom.Number;
	bHasBeenSet = CopyFrom.bHasBeenSet;
	return *this;
}

CRE_ID::CRE_ID(String Name)
	: bHasBeenSet(true)
{
	Number = crc32(Name.c_str(), Name.length());

	//Check if we have this number yet...
	Map<IDNum_t, String>& Map = CRE_ObjectIDRegistry::GetMap();
	if (!Map.contains(Number))
	{
		Map.emplace(Number, Name);
	}
}

CRE_ID::CRE_ID() : Number(0), bHasBeenSet(false) { }
