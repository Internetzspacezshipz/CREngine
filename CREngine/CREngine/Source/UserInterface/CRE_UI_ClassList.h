#pragma once
#include "UserInterface/CRE_UI_Base.h"

//Allows creation of new assets.
class CRE_UI_ClassList : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_ClassList, CRE_UI_Base);

	virtual void DrawUI() override;

	CRE_ClassBase* ShowTable_Classes(CRE_ClassBase* Class);
};

