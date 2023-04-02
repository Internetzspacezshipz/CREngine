#pragma once
#include "UserInterface/CrUI_Base.h"

//Allows creation of new assets.
class CrUI_ClassList : public CrUI_Base
{
	DEF_CLASS(CrUI_ClassList, CrUI_Base);

	virtual void DrawUI() override;
	void RecurseClass_Table(CrClass* Class, CrClass*& WantsToSpawn);
	CrClass* ShowTable_Classes(CrClass* Class);

	bool bShowDataOnlyClasses = false;
	bool bShowTransientClasses = false;
};

