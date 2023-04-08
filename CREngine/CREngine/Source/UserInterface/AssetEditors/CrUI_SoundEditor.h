#pragma once
#include "CrUI_Editor_AssetBase.h"
#include "CrFieldEditor.inl"

class CrUI_SoundEditor : public CrUI_Editor_AssetBase
{
	DEF_CLASS(CrUI_SoundEditor, CrUI_Editor_AssetBase);

	virtual void DrawUI() override;
};
