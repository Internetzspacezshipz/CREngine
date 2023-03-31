#pragma once
#include "CrManagedObject.hpp"

//include this here in order to ensure we have imgui for child classes.
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_stdlib.h"

//Basic user interface object, with the virtual function DrawUI.
class CrUI_Base : public CrManagedObject
{
	DEF_CLASS(CrUI_Base, CrManagedObject);

	virtual ~CrUI_Base();

	//Main function to override for drawing UI.
	virtual void DrawUI();

	bool bOpen = true;
	String WindowTitle;

	//Removes UI - has default implementation
	//passing bPromptAllowed == true will allow the object to create a popup before being destroyed to ask if the user is sure.
	//Default implementation does not care about bPromptAllowed.
	virtual void RemoveUI(bool bPromptAllowed = false);
	forceinline void RemoveUIWithPrompt() { RemoveUI(true); };

	//Override construct to inject this UI into the engine.
	virtual void Construct() override;
};


