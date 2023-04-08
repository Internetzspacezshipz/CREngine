#include "CrAssetList.h"
#include <iostream>

REGISTER_CLASS(CrAssetList);

CrAssetList::~CrAssetList()
{
	Objects.clear();
}

//void CrAssetList::Serialize(bool bSerializing, nlohmann::json& TargetJson)
//{
//	Super::Serialize(bSerializing, TargetJson);
//
//	CrSerialization& Serializer = CrSerialization::Get();
//
//	//bSaveWhenSerialized should be done before the objects array in case we need to load them...
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bSaveWhenSerialized);
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, bLoadWhenDeserialized);
//
//	if (bLoadWhenDeserialized && !bSerializing)
//	{
//		LoadArray(Objects);
//	}
//
//	JSON_SERIALIZE_VARIABLE(TargetJson, bSerializing, Objects);
//
//	if (bSaveWhenSerialized && bSerializing)
//	{
//		SaveArraySafe(Objects);
//	}
//}
//