#pragma once

#include "CRE_ManagedObject.hpp"

//A list of assets that should be loaded from a Json file at AssetListPath.
//These objects should be referenced in the manifest.
//This destroying this class will destroy live owned assets referenced by it.
class CRE_AssetList : public CRE_ManagedObject
{
	DEF_CLASS(CRE_AssetList, CRE_ManagedObject);

	virtual ~CRE_AssetList();

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override;

	std::filesystem::path AssetListPath;

	std::vector<std::shared_ptr<CRE_ManagedObject>> LoadedObjects;
};

REGISTER_CLASS(CRE_AssetList, CRE_ManagedObject);