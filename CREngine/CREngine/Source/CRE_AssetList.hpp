#pragma once

#include "CRE_ManagedObject.hpp"
#include "CRE_Loadable.hpp"

//A list of assets that should be loaded from a Json file at AssetListPath.
//Destroying this object will destroy live owned assets referenced by it.
//This asset controls serialization of the objects in LoadedObjects array
class CRE_AssetList : public CRE_ManagedObject
{
	DEF_CLASS(CRE_AssetList, CRE_ManagedObject);

	virtual ~CRE_AssetList();

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	bool bSaveWhenSerialized = true;
	bool bLoadWhenDeserialized = true;

	Array<CRE_LoadableBase> Objects;
};

