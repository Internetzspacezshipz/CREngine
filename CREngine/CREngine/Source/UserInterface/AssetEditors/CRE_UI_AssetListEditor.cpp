#include "CRE_UI_AssetListEditor.h"
#include "CRE_Globals.h"
#include "CRE_App.hpp"
#include "CRE_AssetList.hpp"

//Basic objects we can edit.
#include "BasicObjects/CRE_Texture.hpp"
#include "BasicObjects/CRE_Mesh.hpp"

//Matching UI style helpers
#include "UserInterface/CRE_UIStyles.h"

REGISTER_CLASS(CRE_UI_AssetListEditor);

ADD_UI_EDITOR(CRE_AssetList, CRE_UI_AssetListEditor);

//Returns true to ask for deletion.
bool ShowObjectInfo(CRE_Loadable<CRE_ManagedObject>& Object)
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
		String StringName = ID.GetString();
		if (ImGui::InputText("Object Instance", &StringName, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
		{
			Object->Rename(StringName);
		}

		ImGui::Text("Class: %s", Object->GetClass().GetString().c_str());

		CRE_EditorUIManager& EditUIMan = CRE_EditorUIManager::Get();

		if (EditUIMan.HasEditUI(Object->GetClass()))
		{
			if (ImGui::Button("Open Editor", DefaultButtonSize))
			{
				EditUIMan.MakeEditUI(Object);
			}
		}
	}

	if (Object.IsLoadable() && !Object.IsLoaded())
	{
		ImGui::Text("Object Name: %s", ID.GetString().c_str());
		if (ImGui::Button("Load object", LargeButtonSize))
		{
			Object.Load();
		}
	}
	else if (Object.IsLoaded())
	{
		if (ImGui::Button("Unload object", LargeButtonSize))
		{
			ImGui::OpenPopup("Save before unloading?");
		}

		if (ImGui::BeginPopupModal("Save before unloading?"))
		{
			//Save first, then unload.
			if (ImGui::Button("Save then unload", LargeButtonSize))
			{
				//Save first and then unload.
				Object.Save();
				Object.Unload();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Unload", DefaultButtonSize))
			{
				//Just unload
				Object.Unload();
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel", DefaultButtonSize))
			{
				//Cancel doing anything
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete Object", LargeButtonSize))
		{
			ImGui::OpenPopup("Are you sure?");
		}

		if (ImGui::BeginPopupModal("Are you sure?"))
		{
			if (ImGui::Button("Delete", DefaultButtonSize))
			{
				//Unload obj (remove shared ptr... Maybe we should do some extra checking to see if object is still alive? idk)
				Object.Unload();
				CRE_Serialization::Get().Delete(Object.GetID());
				bShouldDelete = true;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			
			if (ImGui::Button("Cancel", DefaultButtonSize))
			{
				//exit
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Save", LargeButtonSize))
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
			if (ImGui::Button("Overwrite", LargeButtonSize))
			{
				//Unload obj (remove shared ptr... Maybe we should do some extra checking to see if object is still alive? idk)
				Object.Save();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();

			if (ImGui::Button("Cancel", DefaultButtonSize))
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


void CRE_UI_AssetListEditor::DrawUI()
{
	bool bIsOpen = true;
	ImGui::Begin("AssetListEditor", &bIsOpen);
	Super::DrawUI();
	if (bIsOpen)
	{
		auto AssetList = GetEditedAsset<CRE_AssetList>();
		
		
		if (ImGui::Button("Save", DefaultButtonSize))
		{
			//TODO: overwrite warning.
			SaveAsset();
		}

		ImGui::SameLine();

		//if (ImGui::Button("Load", DefaultButtonSize))
		//{
		//	auto Shared = DCast<CRE_ManagedObject>(PinnedAssetList);
		//	Serialization.Reload(Shared, PinnedAssetList->GetID());
		//}


		FilterExisting.Draw("Filter objects by name");

		auto it = AssetList->Objects.begin();
		while (it != AssetList->Objects.end())
		{
			CRE_Loadable<CRE_ManagedObject>& Ob = (*it);

			if (Ob.HasValidID() && FilterExisting.PassFilter(Ob.GetID().GetString().c_str()))
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
	}
	else
	{
		RemoveUIWithPrompt();
	}

	ImGui::End();
}
