#pragma once
#include "BasicObjects/CRE_UI_Base.hpp"
#include "CRE_KeySystem.hpp"

class CRE_AssetList;

class CRE_UI_AssetListEditor : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_AssetListEditor, CRE_UI_Base);

	virtual ~CRE_UI_AssetListEditor();

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override {};
	virtual void DrawUI() override;
	virtual void Construct() override;

	CRE_ClassBase* ShowTable_Classes(CRE_ClassBase* Class);

	bool bIsOpen = false;
	bool bOpenDemo = false;

	KeySubscriber_wp OpenKeyBind;
	CRE_AssetList* CurrentAssetList;

private:
	//Text filter for existing objects
	ImGuiTextFilter FilterExisting;
};
