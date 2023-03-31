#pragma once
#include "UserInterface/CrUI_Base.h"

//Allows creation of new assets.
class CrUI_ClassList : public CrUI_Base
{
	DEF_CLASS(CrUI_ClassList, CrUI_Base);

	virtual void DrawUI() override;

	CrClass* ShowTable_Classes(CrClass* Class);
};

