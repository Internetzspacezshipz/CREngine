#pragma once
#include "BasicObjects/CRE_UI_Base.hpp"

class CRE_AssetList;

class CRE_UI_AssetListEditor : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_AssetListEditor, CRE_UI_Base);

	virtual ~CRE_UI_AssetListEditor()
	{}

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override {};

	virtual void DrawUI() override;

	bool bIsOpen = false;
	CRE_AssetList* CurrentAssetList;
};


