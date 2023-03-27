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
	CRE_KeySystem* KeySys = CRE_Globals::GetKeySystemPointer();
	KeySys->BindToKeys({SDLK_LCTRL, SDLK_s},
		[this](bool bPressed)
		{
			if (bPressed)
			{
				SaveAsset();
			}
		});

	ActiveEditFilename = CurrentEditedAsset.GetID().GetString();
}

void CRE_UI_Editor_AssetBase::DrawUI()
{
	CRE_Serialization& Serial = CRE_Serialization::Get();

	CRE_ID EditedID = CurrentEditedAsset.GetID();

	//Moves things
	String CurRefString = EditedID.GetString();

	//Edit filename. Popup opens in PopupUI() func
	if (ImGui::InputText("Filename", &ActiveEditFilename, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		if (Serial.Exists(ActiveEditFilename))
		{
			ImGui::OpenPopup("Overwrite?");
		}
		else
		{
			Serial.Move(EditedID, ActiveEditFilename);
			CurrentEditedAsset.Rename(ActiveEditFilename);
			CurrentEditedAsset.Save();
		}
	}

	PopupUI();

	//Only use EditField for things we don't really care about changing that much
	//if (EditField(CurrentEditedAsset) == CRE_FieldEditorReturn_WasEdited)
	//{
	//	MarkAssetNeedsSave();
	//}
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

void CRE_UI_Editor_AssetBase::SaveAsset()
{
	CurrentEditedAsset.Save();
	bWantsSave = false;
}

void CRE_UI_Editor_AssetBase::SetEditedAsset(CRE_Loadable<CRE_ManagedObject> NewAsset)
{
	NewAsset.LoadOrCreate();
	CurrentEditedAsset = NewAsset;
}

void CRE_UI_Editor_AssetBase::PopupUI()
{
	CRE_Serialization& Serial = CRE_Serialization::Get();

	if (ImGui::BeginPopupModal("Unsaved assets"))
	{
		//Save first, then unload.
		if (ImGui::Button("Save"))
		{
			SaveAsset();
			RemoveUI();
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

CRE_EditorUIManager& CRE_EditorUIManager::Get()
{
	static CRE_EditorUIManager Manager;
	return Manager;
}

bool CRE_EditorUIManager::MakeEditUI(CRE_Loadable<CRE_ManagedObject> Object)
{
	if (Object.IsLoadedOrLoadable())
	{
		ClassGUID EditUIClass = FindUIClass(Object.Get<true>()->GetClass());
		if (EditUIClass.IsValidID())
		{
			auto NewUI = DCast<CRE_UI_Editor_AssetBase>(CRE_Globals::GetAppPointer()->MakeUI(EditUIClass));

			assert(NewUI);

			//Add the edited object to the UI.
			NewUI->SetEditedAsset(Object);
			return true;
		}
	}
	return false;
}

bool CRE_EditorUIManager::HasEditUI(ClassGUID Class)
{
	return FindUIClass(Class).IsValidID();
}

ClassGUID CRE_EditorUIManager::FindUIClass(ClassGUID Class)
{
	auto Found = AssetToUIClass.find(Class);
	if (Found == AssetToUIClass.end())
	{
		return ClassGUID();
	}
	return Found->second;
}
