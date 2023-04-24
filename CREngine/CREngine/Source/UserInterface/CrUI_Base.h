#pragma once
#include "CrManagedObject.h"

//include this here in order to ensure we have imgui for child classes.
#include "ThirdParty/ThirdPartyLibs.h"

//Basic user interface object, with the virtual function DrawUI.
class CrUI_Base : public CrManagedObject
{
	DEF_CLASS(CrUI_Base, CrManagedObject);

	CrUI_Base();
	virtual ~CrUI_Base();

	virtual void Start() override;

	//Main function to override for drawing UI.
	virtual void DrawUI();

	bool bOpen = true;
	String WindowTitle;

	//Children of this editor window - this allows inline windows to be created under this one for editing things such as components.
	//You must decide when these windows have their DrawUI function called though!
	Array<SP<CrUI_Base>> Children;
	WP<CrUI_Base> Parent;

	//Removes UI - has default implementation
	//passing bPromptAllowed == true will allow the object to create a popup before being destroyed to ask if the user is sure.
	//Default implementation does not care about bPromptAllowed.
	virtual void RemoveUI(bool bPromptAllowed = false);
	forceinline void RemoveUIWithPrompt() { RemoveUI(true); };
};


