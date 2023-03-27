#pragma once
#include "CRE_ManagedObject.hpp"

//include this here in order to ensure we have imgui for child classes.
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_stdlib.h"

//Basic user interface object, with the virtual function DrawUI.
class CRE_UI_Base : public CRE_ManagedObject
{
	DEF_CLASS(CRE_UI_Base, CRE_ManagedObject);

	virtual ~CRE_UI_Base();

	virtual void Serialize(bool bSerializing, nlohmann::json& TargetJson) override {};
	virtual void DrawUI();
	//Removes UI - has default implementation
	//passing bPromptAllowed == true will allow the object to create a popup before being destroyed to ask if the user is sure.
	//Default implementation does not care about bPromptAllowed.
	virtual void RemoveUI(bool bPromptAllowed = false);

	//Override construct to inject this UI into the engine.
	virtual void Construct() override;
};


