#include "CrUI_AssetList.h"
#include "AssetEditors/CrEditorUIManager.h"
#include "CrUIStyles.h"

REGISTER_CLASS(CrUI_AssetList);

void CrUI_AssetList::DrawUI()
{
	ImGui::Begin(WindowTitle, &bOpen);
	ImGui::SetWindowSize(DefaultEditorWindowSize, ImGuiCond_Once);


	CrSerialization Serial = CrSerialization::Get();

	bool bDisableBack = CurrentDirectory == Serial.GetBaseAssetPath();
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

	CrClass* WantsToSpawn = nullptr;

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
				//DirPath.replace_extension();
				String NoExt = DirPath.generic_string();
				
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
				if (Serial.IsSupportedFileType(DirPath))
				{
					if (ImGui::Button("Open in editor"))
					{
						auto Loaded = CrSerialization::Get().Load(DirPath);
						if (Loaded)
						{
							CrEditorUIManager& UIMan = CrEditorUIManager::Get();
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

void CrUI_AssetList::Construct()
{
	Super::Construct();
	CurrentDirectory = CrSerialization::Get().GetBaseAssetPath();
}
