#pragma once
#include "CrUI_Editor_AssetBase.h"

class CrAssetList;

class CrUI_AssetListEditor : public CrUI_Editor_AssetBase
{
	DEF_CLASS(CrUI_AssetListEditor, CrUI_Editor_AssetBase);

	virtual void DrawUI() override;

private:
	//Text filter for existing objects
	ImGuiTextFilter FilterExisting;
};
