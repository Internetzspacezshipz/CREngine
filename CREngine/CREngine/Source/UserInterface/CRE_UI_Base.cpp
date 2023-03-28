#include "CRE_UI_Base.h"
#include "CRE_Globals.h"
#include "CRE_App.hpp"

//Styles
#include "UserInterface/CRE_UIStyles.h"

REGISTER_CLASS(CRE_UI_Base);

CRE_UI_Base::~CRE_UI_Base()
{

}

void CRE_UI_Base::DrawUI()
{
	ImGui::Begin("Error - CRE_UI_Base created when it should never be created.");
	ImGui::SetWindowSize(ImVec2(200.f,200.f), 0);
	ImGui::End();
}

void CRE_UI_Base::RemoveUI(bool bPromptAllowed)
{
	CRE_Globals::GetAppPointer()->RemoveUI(GetID());
}

void CRE_UI_Base::Construct()
{
	WindowTitle = GetID().GetString().c_str();
}
