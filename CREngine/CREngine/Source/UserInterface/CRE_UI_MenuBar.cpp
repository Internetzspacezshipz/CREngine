#include "CRE_UI_MenuBar.hpp"
#include "CRE_App.hpp"
#include "CRE_Globals.h"
#include "CRE_UI_ClassList.h"
#include "CRE_UI_AssetList.h"

REGISTER_CLASS(CRE_UI_MenuBar);

void CRE_UI_MenuBar::DrawUI()
{
	if (bIsOpen)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Class Selector Window"))
				{
					CRE_App* App = CRE_Globals::GetAppPointer();
					auto Item = App->MakeUI<CRE_UI_ClassList>();
				}

				if (ImGui::MenuItem("Asset Selector Window"))
				{
					CRE_App* App = CRE_Globals::GetAppPointer();
					auto Item = App->MakeUI<CRE_UI_AssetList>();
				}

				if (ImGui::MenuItem("ImGui demo window"))
				{
					bOpenDemo = true;
				}


				//Add things
				ImGui::EndMenu();
			}
			//if (ImGui::BeginMenu("Edit"))
			//{
			//	if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
			//	if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
			//	ImGui::Separator();
			//	if (ImGui::MenuItem("Cut", "CTRL+X")) {}
			//	if (ImGui::MenuItem("Copy", "CTRL+C")) {}
			//	if (ImGui::MenuItem("Paste", "CTRL+V")) {}
			//	ImGui::EndMenu();
			//}
			ImGui::EndMainMenuBar();
		}
	}

	//Open demo
	if (bOpenDemo)
	{
		ImGui::ShowDemoWindow(&bOpenDemo);
	}
}

void CRE_UI_MenuBar::Construct()
{
	CRE_KeySystem* KeySystem = CRE_Globals::GetKeySystemPointer();

	OpenKeyBind = KeySystem->BindToKey(SDLK_BACKQUOTE,
		[this](bool bButtonDown)
		{
			if (bButtonDown)
			{
				bIsOpen = !bIsOpen;
			}
		});
}
