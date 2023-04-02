#include "CrUI_Base.h"
#include "CrGlobals.h"
#include "CrApp.hpp"

//Styles
#include "UserInterface/CrUIStyles.h"

REGISTER_CLASS(CrUI_Base);

CrUI_Base::~CrUI_Base()
{

}

void CrUI_Base::DrawUI()
{
	ImGui::Begin("Error - CrUI_Base created when it should never be created.");
	ImGui::SetWindowSize(ImVec2(200.f,200.f), 0);
	ImGui::End();
}

void CrUI_Base::RemoveUI(bool bPromptAllowed)
{
	CrGlobals::GetAppPointer()->RemoveUI(GetID());
}

void CrUI_Base::Construct()
{
	WindowTitle = String(GetID().GetString().data());
}
