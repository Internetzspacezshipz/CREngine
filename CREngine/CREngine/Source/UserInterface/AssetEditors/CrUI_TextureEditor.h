#pragma once
#include "CrUI_Editor_AssetBase.h"
#include "CrFieldEditor.inl"

class CrUI_TextureEditor : public CrUI_Editor_AssetBase
{
	DEF_CLASS(CrUI_TextureEditor, CrUI_Editor_AssetBase);

	virtual void DrawUI() override;
};
