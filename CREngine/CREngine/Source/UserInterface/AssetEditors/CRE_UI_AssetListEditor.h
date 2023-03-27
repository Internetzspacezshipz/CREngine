#pragma once
#include "CRE_UI_Editor_AssetBase.h"

class CRE_AssetList;

class CRE_UI_AssetListEditor : public CRE_UI_Editor_AssetBase
{
	DEF_CLASS(CRE_UI_AssetListEditor, CRE_UI_Editor_AssetBase);

	virtual void DrawUI() override;

private:
	//Text filter for existing objects
	ImGuiTextFilter FilterExisting;
};
