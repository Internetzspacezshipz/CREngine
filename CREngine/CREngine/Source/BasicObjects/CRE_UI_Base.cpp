#include "CRE_UI_Base.hpp"
#include "CRE_Globals.hpp"

REGISTER_CLASS(CRE_UI_Base, CRE_ManagedObject);

CRE_UI_Base::~CRE_UI_Base()
{

}

void CRE_UI_Base::DrawUI()
{
	ImGui::Begin("Error - CRE_UI_Base created when it should never be created.");
	ImGui::SetWindowSize(ImVec2(200.f,200.f), 0);
	ImGui::End();
}

void CRE_UI_Base::Construct()
{

}
