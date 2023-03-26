#pragma once
#include "BasicObjects/CRE_UI_Base.hpp"
#include "CRE_KeySystem.hpp"

class CRE_AssetList;

class CRE_UI_AssetListEditor : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_AssetListEditor, CRE_UI_Base);

	virtual void DrawUI() override;

	CRE_ClassBase* ShowTable_Classes(CRE_ClassBase* Class);

	//Set this to false to close this window.
	bool bIsOpen = true;

	WP<CRE_AssetList> CurrentAssetList;

private:
	//Text filter for existing objects
	ImGuiTextFilter FilterExisting;
};
