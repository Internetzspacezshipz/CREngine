#pragma once
#include "CRE_UI_Base.h"
#include "CRE_KeySystem.hpp"

//Menu bar that allows us to open different debug/tuning UI.
class CRE_UI_MenuBar : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_MenuBar, CRE_UI_Base);

	virtual void DrawUI() override;
	virtual void Construct() override;

	bool bIsOpen = false;
	bool bOpenDemo = false;

	SP<KeySubscriber> OpenKeyBind;
};
