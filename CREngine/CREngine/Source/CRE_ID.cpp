#include "CRE_ID.hpp"
#include "CRE_SimpleHashes.h"

Map<IDNum_t, std::string>& CRE_ObjectIDRegistry::GetMap()
{
	static Map<IDNum_t, std::string> Map;
	return Map;
}

std::string CRE_ObjectIDRegistry::CreateUniqueString(const std::string& In)
{
	Map<IDNum_t, std::string>& Map = GetMap();
	int Index = 0;
	std::string UniqueString = In;
	do
	{
		UniqueString = In + std::format("_{}", Index++);
	} while (Map.find(crc32(UniqueString)) != Map.end());
	return UniqueString;
}

std::string CRE_ID::GetString() const
{
	std::string Out = "";
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

CRE_ID::CRE_ID(std::string Name)
	: bHasBeenSet(true)
{
	Number = crc32(Name.c_str(), Name.length());

	//Check if we have this number yet...
	Map<IDNum_t, std::string>& Map = CRE_ObjectIDRegistry::GetMap();
	if (!Map.contains(Number))
	{
		Map.emplace(Number, Name);
	}
}

CRE_ID::CRE_ID() : Number(0), bHasBeenSet(false) { }
