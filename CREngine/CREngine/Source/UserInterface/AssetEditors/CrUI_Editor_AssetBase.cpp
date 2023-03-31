#include "CrUI_Editor_AssetBase.h"

//Matching UI style helpers
#include "UserInterface/CrUIStyles.h"

//For adding new UI.
#include "CrGlobals.h"
#include "CrApp.hpp"
#include "CrKeySystem.hpp"

#include "CrFieldEditor.inl"



REGISTER_CLASS(CrUI_Editor_AssetBase);

void CrUI_Editor_AssetBase::Construct()
{
	Super::Construct();
	CrKeySystem* KeySys = CrGlobals::GetKeySystemPointer();
	KeySys->BindToKeys({SDLK_LCTRL, SDLK_s},
		[this](bool bPressed)
		{
			if (bPressed)
			{
				SaveAssetWithPrompts();
			}
		});
}

void CrUI_Editor_AssetBase::DrawUI()
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

void CrUI_Editor_AssetBase::RemoveUI(bool bPromptAllowed)
{
	if (bPromptAllowed && bWantsSave)
	{
		ImGui::OpenPopup("Unsaved assets");
		return;
	}
	CurrentEditedAsset.Unload();
	Super::RemoveUI();
}

void CrUI_Editor_AssetBase::SaveAssetWithPrompts()
{
	if (CurrentEditedAsset.IsLoadedOrLoadable() == false)
	{
		return;
	}

	CrSerialization& Serial = CrSerialization::Get();

	CrAssetReference OldRef = CurrentEditedAsset->MakeAssetReference();

	//copy the ref and change the active edit filename
	CrAssetReference NewRef = OldRef;
	OldRef.AssetID = ActiveEditFilename;


	if (Serial.Exists(NewRef))
	{
		ImGui::OpenPopup("Overwrite?");
	}
	else
	{
		Serial.Move(OldRef, NewRef);
		CurrentEditedAsset.Rename(ActiveEditFilename);
		//Resaves the asset if needed.
		CurrentEditedAsset.Save();
		bWantsSave = false;
	}
}

ImGuiWindowFlags CrUI_Editor_AssetBase::GetWindowFlags()
{
	return bWantsSave ? ImGuiWindowFlags_UnsavedDocument : 0;
}

void CrUI_Editor_AssetBase::SetEditedAsset(CrLoadable<CrManagedObject> NewAsset)
{
	NewAsset.LoadOrCreate();
	CurrentEditedAsset = NewAsset;
}

//TODO - move this to be after all other UI code.
void CrUI_Editor_AssetBase::PopupUI()
{
	CrSerialization& Serial = CrSerialization::Get();

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
			auto ARef = CurrentEditedAsset->MakeAssetReference();
			auto NewRef = ARef;
			NewRef.AssetID = ActiveEditFilename;

			Serial.Move(ARef, NewRef);
			CurrentEditedAsset.Rename(NewRef.AssetID);
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

