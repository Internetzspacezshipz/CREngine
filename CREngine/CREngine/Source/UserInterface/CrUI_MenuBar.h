#pragma once
#include "CrUI_Base.h"
#include "CrKeySystem.h"

//Menu bar that allows us to open different debug/tuning UI.
class CrUI_MenuBar : public CrUI_Base
{
	DEF_CLASS(CrUI_MenuBar, CrUI_Base);

	CrUI_MenuBar();
	virtual void DrawUI() override;

	bool bIsOpen = false;
	bool bOpenDemo = false;

	SP<KeySubscriber> OpenKeyBind;
};
