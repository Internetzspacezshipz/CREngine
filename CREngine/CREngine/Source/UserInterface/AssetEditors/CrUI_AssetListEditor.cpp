#include "CrUI_AssetListEditor.h"
#include "CrGlobals.h"
#include "CrApp.hpp"
#include "CrAssetList.hpp"

//Basic objects we can edit.
#include "BasicObjects/CrTexture.h"
#include "BasicObjects/CrMesh.h"

//Matching UI style helpers
#include "UserInterface/CrUIStyles.h"

#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_AssetListEditor);

ADD_UI_EDITOR(CrAssetList, CrUI_AssetListEditor);

//Returns true to ask for deletion.
bool ShowObjectInfo(CrLoadable<CrManagedObject>& Object)
{
	bool bShouldDelete = false;
	auto ID = Object.GetID();
	ImGui::PushID(ID.GetNumber());
	ImGui::Separator();

	if (!Object.IsLoadedOrLoadable())
	{
		//Remove object if not loadable
		bShouldDelete = true;
	}

	//Check again if object is loaded, since it might have been unloaded in the earlier IsLoaded scope.
	if (Object.IsLoaded() && !bShouldDelete)
	{
		String StringName = String(ID.GetString());
		if (ImGui::InputText("Object Instance", &StringName, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
		{
			Object->Rename(StringName);
		}

		ImGui::Text("Class: %s", Object->GetClass().GetString().data());

		CrEditorUIManager& EditUIMan = CrEditorUIManager::Get();

		if (ImGui::Button("Open Editor"))
		{
			EditUIMan.MakeEditUI(Object);
		}
	}

	if (Object.IsLoadable() && !Object.IsLoaded())
	{
		ImGui::Text("Object Name: %s", ID.GetString().data());
		if (ImGui::Button("Load object"))
		{
			Object.Load();
		}
	}
	else if (Object.IsLoaded())
	{
		if (ImGui::Button("Unload object"))
		{
			ImGui::OpenPopup("Save before unloading?");
		}

		if (ImGui::BeginPopupModal("Save before unloading?"))
		{
			//Save first, then unload.
			if (ImGui::Button("Save then unload"))
			{
				//Save first and then unload.
				Object.Save();
				Object.Unload();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Unload"))
			{
				//Just unload
				Object.Unload();
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

		ImGui::SameLine();

		if (ImGui::Button("Delete Object"))
		{
			ImGui::OpenPopup("Are you sure?");
		}

		if (ImGui::BeginPopupModal("Are you sure?"))
		{
			if (ImGui::Button("Delete"))
			{
				//Unload obj (remove shared ptr... Maybe we should do some extra checking to see if object is still alive? idk)
				CrSerialization::Get().Delete(Object.Get());
				Object.Unload();
				bShouldDelete = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			
			if (ImGui::Button("Cancel"))
			{
				//exit
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Save"))
		{
			if (Object.IsLoadable())
			{
				ImGui::OpenPopup("Overwrite?");
			}
			else
			{
				Object.Save();
			}
		}

		if (ImGui::BeginPopupModal("Overwrite?"))
		{
			if (ImGui::Button("Overwrite"))
			{
				//Unload obj (remove shared ptr... Maybe we should do some extra checking to see if object is still alive? idk)
				Object.Save();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				//exit
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}

	ImGui::Separator();

	ImGui::PopID();
	return bShouldDelete;
}


void CrUI_AssetListEditor::DrawUI()
{
	Super::DrawUI();
	ImGui::Begin(WindowTitle.c_str(), &bOpen);

	auto AssetList = GetEditedAsset<CrAssetList>();

	//if (ImGui::Button("LoadTexture", DefaultButtonSize))
	//{
	//	auto Shared = DCast<CrManagedObject>(PinnedAssetList);
	//	Serialization.Reload(Shared, PinnedAssetList->GetID());
	//}

	String NewAssetName;
	if (ImGui::InputText("NewAssetName", &NewAssetName))
	{
		//Check if the asset already exists in the list.
		bool bExists = false;
		for (auto& Elem : AssetList->Objects)
		{
			if (Elem.GetID().GetString() == NewAssetName)
			{
				bExists = true;
				break;
			}
		}
		if (!bExists)
		{
			ImGui::SameLine();
			if (ImGui::Button("Add Reference"))
			{
				CrLoadable<CrManagedObject> New;
				//New.Set(NewAssetName);
				AssetList->Objects.push_back(New);
			}
		}
	}

	FilterExisting.Draw("Filter objects by name");

	auto it = AssetList->Objects.begin();
	while (it != AssetList->Objects.end())
	{
		CrLoadable<CrManagedObject>& Ob = (*it);

		if (Ob.HasValidID() && FilterExisting.PassFilter(Ob.GetID().GetString().data()))
		{
			if (ShowObjectInfo(Ob))
			{
				it = AssetList->Objects.erase(it);
			}
			else
			{
				it++;
			}
		}
	}

	ImGui::End();
}
