#pragma once

#include "CrManagedObject.h"
#include "CrLoadable.h"

//A list of assets that should be loaded from a Json file at AssetListPath.
//Destroying this object will destroy live owned assets referenced by it.
//This asset controls serialization of the objects in LoadedObjects array
class CrAssetList : public CrManagedObject
{
	DEF_CLASS(CrAssetList, CrManagedObject);

	virtual ~CrAssetList();

	//virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	bool bSaveWhenSerialized = true;
	bool bLoadWhenDeserialized = true;

	Array<CrLoadable<CrManagedObject>> Objects;
};

