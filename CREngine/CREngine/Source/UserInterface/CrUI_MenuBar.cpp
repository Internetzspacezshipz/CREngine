#include "CrUI_MenuBar.h"
#include "CrApp.h"
#include "CrGlobals.h"
#include "CrUI_ClassList.h"
#include "CrUI_AssetList.h"

REGISTER_CLASS(CrUI_MenuBar);

void CrUI_MenuBar::DrawUI()
{
	if (bIsOpen)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Class Selector Window"))
				{
					CrApp* App = CrGlobals::GetAppPointer();
					auto Item = App->MakeUI<CrUI_ClassList>();
				}

				if (ImGui::MenuItem("Asset Selector Window"))
				{
					CrApp* App = CrGlobals::GetAppPointer();
					auto Item = App->MakeUI<CrUI_AssetList>();
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

void CrUI_MenuBar::Construct()
{
	CrKeySystem* KeySystem = CrGlobals::GetKeySystemPointer();

	OpenKeyBind = KeySystem->BindToKey(SDLK_BACKQUOTE,
		[this](bool bButtonDown)
		{
			if (bButtonDown)
			{
				bIsOpen = !bIsOpen;
			}
		});
}
