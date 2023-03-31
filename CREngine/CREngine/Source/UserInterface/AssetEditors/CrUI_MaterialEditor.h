#pragma once
#include "CrUI_Editor_AssetBase.h"

class CrUI_MaterialEditor : public CrUI_Editor_AssetBase
{
	DEF_CLASS(CrUI_MaterialEditor, CrUI_Editor_AssetBase);

	virtual void DrawUI() override;
};
