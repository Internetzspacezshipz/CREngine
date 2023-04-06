#pragma once
#include "UserInterface/CrUI_Base.h"

// Directory tree structure display
class CrUI_AssetList : public CrUI_Base
{
	DEF_CLASS(CrUI_AssetList, CrUI_Base);

	Path CurrentDirectory;

	virtual void DrawUI() override;

	virtual void Construct() override;
};
