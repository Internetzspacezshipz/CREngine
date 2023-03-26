#include "CRE_UI_MenuBar.hpp"
#include "CRE_App.hpp"
#include "CRE_Globals.hpp"
#include "CRE_UI_AssetListEditor.hpp"

REGISTER_CLASS(CRE_UI_MenuBar, CRE_UI_Base);

void CRE_UI_MenuBar::DrawUI()
{
	if (bIsOpen)
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Asset Selector Window"))
				{
					CRE_App* App = CRE_Globals::GetAppPointer();
					App->AddUI(CRE_UI_AssetListEditor::StaticClass(), CRE_ObjectFactory::Get().Create<CRE_UI_AssetListEditor>());
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
