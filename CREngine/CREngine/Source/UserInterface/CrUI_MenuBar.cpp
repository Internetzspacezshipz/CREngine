#include "CrUI_MenuBar.h"
#include "CrApp.h"
#include "CrGlobals.h"
#include "CrUI_ClassList.h"
#include "CrUI_AssetList.h"

#include "CrSerialization.h"
#include "AssetEditors/CrEditorUIManager.h"

REGISTER_CLASS(CrUI_MenuBar);

CrUI_MenuBar::CrUI_MenuBar()
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

void CrUI_MenuBar::DrawUI()
{
	if (bIsOpen)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				CrApp* App = CrGlobals::GetAppPointer();

				if (ImGui::MenuItem("Class Selector Window"))
				{
					App->MakeUI<CrUI_ClassList>();
				}

				if (ImGui::MenuItem("Asset Selector Window"))
				{
					App->MakeUI<CrUI_AssetList>()->SetOnOpenAction(CrAssetListFunctionality::OpenEditor);
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
