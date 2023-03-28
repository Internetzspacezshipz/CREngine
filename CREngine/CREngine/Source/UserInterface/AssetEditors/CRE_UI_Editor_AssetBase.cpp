#include "CRE_UI_Editor_AssetBase.h"

//Matching UI style helpers
#include "UserInterface/CRE_UIStyles.h"

//For adding new UI.
#include "CRE_Globals.h"
#include "CRE_App.hpp"
#include "CRE_KeySystem.hpp"

#include "CRE_FieldEditor.inl"



REGISTER_CLASS(CRE_UI_Editor_AssetBase);

void CRE_UI_Editor_AssetBase::Construct()
{
	Super::Construct();
	CRE_KeySystem* KeySys = CRE_Globals::GetKeySystemPointer();
	KeySys->BindToKeys({SDLK_LCTRL, SDLK_s},
		[this](bool bPressed)
		{
			if (bPressed)
			{
				SaveAssetWithPrompts();
			}
		});
}

void CRE_UI_Editor_AssetBase::DrawUI()
{
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());
	ImGui::SetWindowSize(DefaultEditorWindowSize, ImGuiCond_Once);
	ImGui::SetWindowPos({ 200.f, 50.f }, ImGuiCond_Once);

	if (ActiveEditFilename.size() == 0)
	{
		ActiveEditFilename = CurrentEditedAsset.GetID().GetString();
	}

	//Edit filename. Popup opens in PopupUI() func
	if (ImGui::Button("Save Asset"))
	{
		SaveAssetWithPrompts();
	}

	ImGui::SameLine();

	ImGui::InputText("Filename", &ActiveEditFilename);

	if (ActiveEditFilename != CurrentEditedAsset.GetID().GetString())
	{
		MarkAssetNeedsSave();
	}

	if (!bOpen)
	{
		bOpen = true;
		RemoveUIWithPrompt();
	}

	PopupUI();

	ImGui::End();
}

void CRE_UI_Editor_AssetBase::RemoveUI(bool bPromptAllowed)
{
	if (bPromptAllowed && bWantsSave)
	{
		ImGui::OpenPopup("Unsaved assets");
		return;
	}
	Super::RemoveUI();
}

void CRE_UI_Editor_AssetBase::SaveAssetWithPrompts()
{
	CRE_Serialization& Serial = CRE_Serialization::Get();

	if (Serial.Exists(ActiveEditFilename))
	{
		ImGui::OpenPopup("Overwrite?");
	}
	else
	{
		Serial.Move(CurrentEditedAsset.GetID(), ActiveEditFilename);
		CurrentEditedAsset.Rename(ActiveEditFilename);
		//Resaves the asset if needed.
		CurrentEditedAsset.Save();
		bWantsSave = false;
	}
}

ImGuiWindowFlags CRE_UI_Editor_AssetBase::GetWindowFlags()
{
	return bWantsSave ? ImGuiWindowFlags_UnsavedDocument : 0;
}

void CRE_UI_Editor_AssetBase::SetEditedAsset(CRE_Loadable<CRE_ManagedObject> NewAsset)
{
	NewAsset.LoadOrCreate();
	CurrentEditedAsset = NewAsset;
}

//TODO - move this to be after all other UI code.
void CRE_UI_Editor_AssetBase::PopupUI()
{
	CRE_Serialization& Serial = CRE_Serialization::Get();

	if (ImGui::BeginPopupModal("Unsaved assets"))
	{
		//Save first, then unload.
		if (ImGui::Button("Save"))
		{
			SaveAssetWithPrompts();
			//Save first and then unload.
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Close without saving"))
		{
			RemoveUI();
			//Just unload
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			//Cancel doing anything
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	//NEVER early return from this or it may break things.
	if (ImGui::BeginPopupModal("Overwrite?"))
	{
		//Save first, then unload.
		if (ImGui::Button("Overwrite!"))
		{
			Serial.Move(CurrentEditedAsset.GetID().GetString(), ActiveEditFilename);
			CurrentEditedAsset.Rename(ActiveEditFilename);
			CurrentEditedAsset.Save();
			bWantsSave = false;

			//Save first and then unload.
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			ActiveEditFilename = CurrentEditedAsset.GetID().GetString();
			//Cancel doing anything
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

