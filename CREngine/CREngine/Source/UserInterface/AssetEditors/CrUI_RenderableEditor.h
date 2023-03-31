#pragma once
#include "CrUI_Editor_AssetBase.h"

class CrUI_RenderableEditor : public CrUI_Editor_AssetBase
{
	DEF_CLASS(CrUI_RenderableEditor, CrUI_Editor_AssetBase);

	virtual void DrawUI() override;
};
