#pragma once
#include "UserInterface/CRE_UI_Base.h"

// Directory tree structure display
class CRE_UI_AssetList : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_AssetList, CRE_UI_Base);

	Path CurrentDirectory;

	virtual void DrawUI() override;

	virtual void Construct() override;
};

