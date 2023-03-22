#include "CRE_UI_AssetListEditor.hpp"
#include "CRE_Globals.hpp"
#include "CRE_App.hpp"
#include "CRE_AssetList.hpp"

#include <stdlib.h>
#include <SDL.h>

REGISTER_CLASS(CRE_UI_AssetListEditor, CRE_UI_Base);

//Remove keybind here.
CRE_UI_AssetListEditor::~CRE_UI_AssetListEditor()
{
	OpenKeyBind.lock()->Remove();
}

void RecurseClass(CRE_ClassBase* Class)
{
	ImGui::PushID(Class);

	ImGui::Text(Class->GetClassFriendlyName().c_str());
	ImGui::SameLine();
	ImGui::Value("ClassID", Class->GetClassGUID());

	ImGui::Indent(20.f);

	for (CRE_ClassBase* Child : Class->GetChildren())
	{
		RecurseClass(Child);
	}

	ImGui::Unindent(20.f);

	ImGui::PopID();
}

void CRE_UI_AssetListEditor::DrawUI()
{
	if (bIsOpen)
	{
		CRE_Serialization& Serialization = CRE_Serialization::Get();
		CRE_App* App = CRE_Globals::GetAppPointer();

		if (CurrentAssetList == nullptr)
		{
			CurrentAssetList = App->GetRootAssetList();
		}

		std::string FileName = CurrentAssetList->AssetListPath.filename().generic_string();
		std::string FileNameCpy = FileName;

		ImGui::Begin("AssetListEditor", &bIsOpen);

		//Disallow renaming of main asset.
		if (CurrentAssetList != App->GetRootAssetList())
		{
			ImGui::InputText("Filename:", &FileName);
			if (FileName != FileNameCpy)
			{
				CurrentAssetList->AssetListPath = FileName;
			}
		}

		if (ImGui::Button("Save", { 50.f,20.f }))
		{
			nlohmann::json OutJson;
			CurrentAssetList->Serialize(true, OutJson);

			Serialization.SaveJsonToFile(CurrentAssetList->AssetListPath, OutJson);
			//Save logic.
		}

		ImGui::SameLine();

		if (ImGui::Button("Load", { 50.f,20.f }))
		{
			nlohmann::json OutJson = Serialization.LoadFileToJson(CurrentAssetList->AssetListPath);
			CurrentAssetList->Serialize(false, OutJson);
		}

		if (ImGui::CollapsingHeader("List Asset"))
		{
			CRE_ClassBase* Base = CRE_ObjectFactory::Get().GetClass(CRE_ManagedObject::StaticClass());

			RecurseClass(Base);
		}

		for (auto& Elem : CurrentAssetList->LoadedObjects)
		{
			//Do thing
			Elem->GetClass();
		}

		ImGui::End();
	}
}

//Add keybind to open menu here.
void CRE_UI_AssetListEditor::Construct()
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
