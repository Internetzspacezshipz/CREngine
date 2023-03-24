#include "CRE_ManagedObject.hpp"

REGISTER_CLASS(CRE_ManagedObject, void);

void CRE_ManagedObject::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, ID);
	//Manually serialize the class only when 
	if (bSerializing)
	{
		//Must do this to avoid binding reference - we're only allowing outgoing anyway.
		std::string OutStr = GetClass().GetString();
		JSON_SERIALIZE_VARIABLE_STRNAME<std::string>(TargetJson, bSerializing, OutStr, CLASS_JSON_VALUE);
	}
}

std::unordered_map<IDNum_t, std::string>& CRE_ObjectIDRegistry::GetMap()
{
	static std::unordered_map<IDNum_t, std::string> Map;
	return Map;
}

std::string CRE_ObjectIDRegistry::CreateUniqueString(const std::string& In)
{
	std::unordered_map<IDNum_t, std::string>& Map = GetMap();
	int Index = 0;
	std::string UniqueString = In;
	do
	{
		UniqueString = In + std::format("_{}", Index++);
	} while (Map.find(crc32(UniqueString)) != Map.end());
	return UniqueString;
}

std::string CRE_ObjID::GetString() const
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

bool CRE_ObjID::IsValidID() const
{
	return bHasBeenSet && CRE_ObjectIDRegistry::GetMap().contains(Number);
}

CRE_ObjID& CRE_ObjID::operator=(const CRE_ObjID& CopyFrom)
{
	assert(CopyFrom.bHasBeenSet);
	Number = CopyFrom.Number;
	bHasBeenSet = CopyFrom.bHasBeenSet;
	return *this;
}

CRE_ObjID::CRE_ObjID(std::string Name) 
	: bHasBeenSet(true)
{
	Number = crc32(Name.c_str(), Name.length());

	//Check if we have this number yet...
	std::unordered_map<IDNum_t, std::string>& Map = CRE_ObjectIDRegistry::GetMap();
	if (!Map.contains(Number))
	{
		Map.emplace(Number, Name);
	}
}

CRE_ObjID::CRE_ObjID() : Number(0), bHasBeenSet(false) { }
