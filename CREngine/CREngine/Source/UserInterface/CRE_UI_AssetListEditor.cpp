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
	ImGui::Value("ClassID", Class->GetClassGUID());

	//ImGui::SameLine();
	ImGui::Text(Class->GetClassFriendlyName().c_str());

	ImGui::Indent(20.f);

	for (CRE_ClassBase* Child : Class->GetChildren())
	{
		RecurseClass(Child);
	}

	ImGui::Unindent(20.f);

	ImGui::PopID();
}


void RecurseClass_Table(CRE_ClassBase* Class)
{
	ImGui::PushID(Class);
	//ImGui::SameLine();

	ImGui::Indent(20.f);

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	//Class name + 
	std::string GUIDStr = std::format("{}", Class->GetClassGUID());

	if (ImGui::Button(GUIDStr.c_str(), {80.f, ImGui::GetTextLineHeightWithSpacing() }))
	{
		ImGui::LogToClipboard();
		ImGui::LogText(GUIDStr.c_str());
		ImGui::LogFinish();
	}

	ImGui::TableNextColumn();

	ImGui::Text(Class->GetClassFriendlyName().c_str());


	for (CRE_ClassBase* Child : Class->GetChildren())
	{
		RecurseClass_Table(Child);
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

		ImGui::Checkbox("IMGUI_DEMO", &bOpenDemo);
		if (bOpenDemo)
		{
			ImGui::ShowDemoWindow(&bOpenDemo);
		}

		if (ImGui::CollapsingHeader("List Asset"))
		{
			CRE_ClassBase* Base = CRE_ObjectFactory::Get().GetClass(CRE_ManagedObject::StaticClass());

			const int column_count = 2;
			const char* column_names[column_count] = { "Class ID", "Class Name" };
			static ImGuiTableColumnFlags column_flags[column_count] = { ImGuiTableColumnFlags_DefaultSort, ImGuiTableColumnFlags_None };

			const ImGuiTableFlags flags
				= ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY
				| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV
				| ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable;

			ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 9);

			if (ImGui::BeginTable("table_columns_flags", column_count, flags, outer_size))
			{
				for (int column = 0; column < column_count; column++)
				{
					ImGui::TableSetupColumn(column_names[column], column_flags[column]);
				}
				ImGui::TableHeadersRow();

				//float indent_step = (float)((int)ImGui::GetTextLineHeightWithSpacing() / 2);

				RecurseClass_Table(Base);

				ImGui::EndTable();
			}
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
