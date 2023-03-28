#include "CRE_UI_AssetList.h"
#include "AssetEditors/CRE_EditorUIManager.h"
#include "CRE_UIStyles.h"

REGISTER_CLASS(CRE_UI_AssetList);

void CRE_UI_AssetList::DrawUI()
{
	ImGui::Begin(WindowTitle.c_str(), &bOpen);
	ImGui::SetWindowSize(DefaultEditorWindowSize, ImGuiCond_Once);

	bool bDisableBack = CurrentDirectory == CRE_Serialization::Get().GetBaseAssetPath();
	if (bDisableBack)
	{
		ImGui::BeginDisabled();
	}

	if (ImGui::Button("Back"))
	{
		CurrentDirectory = CurrentDirectory.parent_path();
	}

	if (bDisableBack)
	{
		ImGui::EndDisabled();
	}

	//table setup
	const int ColumnCount = 2;
	const char* ColumnNames[ColumnCount] = { "Name", "Actions" };
	static ImGuiTableColumnFlags ColumnFlags[ColumnCount] = { ImGuiTableColumnFlags_DefaultSort, ImGuiTableColumnFlags_None };

	const ImGuiTableFlags TableFlags
		= ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV
		| ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody;

	ImVec2 TableSize = ImVec2(0.0f, 0.f/*ImGui::GetTextLineHeightWithSpacing() * 30*/);

	CRE_ClassBase* WantsToSpawn = nullptr;

	if (ImGui::BeginTable("table_columns_flags", ColumnCount, TableFlags, TableSize))
	{
		for (int Column = 0; Column < ColumnCount; Column++)
		{
			ImGui::TableSetupColumn(ColumnNames[Column], ColumnFlags[Column]);
		}

		ImGui::TableHeadersRow();

		//Main directory iteration loop.
		for (const auto& Dir : std::filesystem::directory_iterator{ CurrentDirectory })
		{
			auto DirPath = Dir.path();
			String DirName = DirPath.filename().generic_string();
			String Extension = DirPath.extension().generic_string();

			ImGui::PushID(DirName.c_str());

			ImGui::TableNextRow();
			ImGui::TableNextColumn();


			if (Dir.is_directory())
			{
				if (ImGui::Button(DirName.c_str()))
				{
					CurrentDirectory = DirPath;
				}
			}
			else
			{
				//Remove ext.
				DirPath.replace_extension();
				String NoExt = DirPath.filename().generic_string();
				

				ImGui::Text(NoExt.c_str());

				ImGui::TableNextColumn();
				if (ImGui::Button("Copy reference"))
				{
					//Do copy to clipboard.
					ImGui::LogToClipboard();
					ImGui::LogText(NoExt.c_str());
					ImGui::LogFinish();
				}
				ImGui::SameLine();
				if (Extension == ".json")
				{
					if (ImGui::Button("Open in editor"))
					{
						auto Loaded = CRE_Serialization::Get().Load(NoExt);
						if (Loaded)
						{
							CRE_EditorUIManager& UIMan = CRE_EditorUIManager::Get();
							UIMan.MakeEditUI(Loaded);
						}
					}
				}
			}
			ImGui::PopID();
		}

		ImGui::EndTable();
	}
	ImGui::End();
	if (!bOpen)
	{
		RemoveUI();
	}
}

void CRE_UI_AssetList::Construct()
{
	Super::Construct();
	CurrentDirectory = CRE_Serialization::Get().GetBaseAssetPath();
}
