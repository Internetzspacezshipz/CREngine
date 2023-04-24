#pragma once
#include "CrUI_Editor_AssetBase.h"

class CrVerse;
class CrUI_AssetList;

class CrUI_VerseEditor : public CrUI_Editor_AssetBase
{
	DEF_CLASS(CrUI_VerseEditor, CrUI_Editor_AssetBase);

	CrUI_VerseEditor();
	virtual ~CrUI_VerseEditor();

	virtual void DrawUI() override;

protected:
	SP<CrUI_AssetList> PopupList;

private:

	//Text filter for existing objects
	ImGuiTextFilter FilterExisting;
};
