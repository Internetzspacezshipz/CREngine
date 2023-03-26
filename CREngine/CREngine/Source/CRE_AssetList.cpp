#include "CRE_AssetList.hpp"
#include <iostream>

REGISTER_CLASS(CRE_AssetList, CRE_ManagedObject);

CRE_AssetList::~CRE_AssetList()
{
	Objects.clear();
}

void CRE_AssetList::Serialize(bool bSerializing, nlohmann::json& TargetJson)
{
	Super::Serialize(bSerializing, TargetJson);

	CRE_Serialization& Serializer = CRE_Serialization::Get();

	//bSaveWhenSerialized should be done before the objects array in case we need to load them...
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bSaveWhenSerialized);
	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bLoadWhenDeserialized);

	if (bLoadWhenDeserialized && !bSerializing)
	{
		LoadArray(Objects);
	}

	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Objects);

	if (bSaveWhenSerialized && bSerializing)
	{
		SaveArraySafe(Objects);
	}
}
