#pragma once
#include "CRE_UI_Editor_AssetBase.h"

class CRE_UI_RenderableEditor : public CRE_UI_Editor_AssetBase
{
	DEF_CLASS(CRE_UI_RenderableEditor, CRE_UI_Editor_AssetBase);

	virtual void DrawUI() override;
};
