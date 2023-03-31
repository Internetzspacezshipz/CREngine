#include "CrUI_ClassList.h"
#include "CrUIStyles.h"

#include <CrGlobals.h>
#include "CrApp.hpp"

#include "AssetEditors/CrUI_Editor_AssetBase.h"
#include "AssetEditors/CrEditorUIManager.h"

REGISTER_CLASS(CrUI_ClassList);

void CrUI_ClassList::DrawUI()
{
	ImGui::Begin(WindowTitle.c_str(), &bOpen);
	CrObjectFactory& OF = CrObjectFactory::Get();

	CrClass* WantsToSpawn = nullptr;

	CrClass* Base = OF.GetClass(CrManagedObject::StaticClass());
	WantsToSpawn = ShowTable_Classes(Base);

	if (WantsToSpawn)
	{
		CrEditorUIManager& UIMan = CrEditorUIManager::Get();
		auto NewInstanceClass = WantsToSpawn->GetClassGUID();
		auto Out = OF.Create(NewInstanceClass);
		UIMan.MakeEditUI(Out);
	}

	if (!bOpen)
	{
		RemoveUIWithPrompt();
	}

	ImGui::End();
}


void RecurseClass(CrClass* Class)
{
	ImGui::PushID(Class);
	ImGui::Value("ClassID", Class->GetClassGUID().GetNumber());

	//ImGui::SameLine();
	ImGui::Text(Class->GetClassName().data());

	ImGui::Indent(20.f);

	for (CrClass* Child : Class->GetChildren())
	{
		RecurseClass(Child);
	}

	ImGui::Unindent(20.f);

	ImGui::PopID();
}

void RecurseClass_Table(CrClass* Class, CrClass*& WantsToSpawn)
{
	ImGui::PushID(Class);
	//ImGui::SameLine();

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	bool bOpen = true;

	if (Class->GetChildren().size())
	{
		bOpen = ImGui::CollapsingHeader(Class->GetClassName().data(),
			ImGuiTreeNodeFlags_DefaultOpen |
			ImGuiTreeNodeFlags_OpenOnArrow |
			ImGuiTreeNodeFlags_OpenOnDoubleClick |
			ImGuiTreeNodeFlags_AllowItemOverlap);
	}
	else
	{
		ImGui::Text(Class->GetClassName().data());
	}


	ImGui::TableNextColumn();

	CrStyles::ButtonStyleStart();

	//Clicking the class name spawns the object.
	if (ImGui::Button(Class->GetClassName().data()))
	{
		WantsToSpawn = Class;
	}

	CrStyles::ButtonStyleEnd();

	ImGui::Indent(20.f);

	if (bOpen)
	{
		for (CrClass* Child : Class->GetChildren())
		{
			RecurseClass_Table(Child, WantsToSpawn);
		}
	}

	ImGui::Unindent(20.f);

	ImGui::PopID();
}

CrClass* CrUI_ClassList::ShowTable_Classes(CrClass* Class)
{
	const int ColumnCount = 2;
	const char* ColumnNames[ColumnCount] = { "Class Name", "Create" };
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

		//float indent_step = (float)((int)ImGui::GetTextLineHeightWithSpacing() / 2);

		RecurseClass_Table(Class, WantsToSpawn);

		ImGui::EndTable();
	}

	return WantsToSpawn;
}

