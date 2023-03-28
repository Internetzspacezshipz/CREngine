#include "CRE_UI_ClassList.h"
#include "CRE_UIStyles.h"

#include <CRE_Globals.h>
#include "CRE_App.hpp"

#include "AssetEditors/CRE_UI_Editor_AssetBase.h"
#include "AssetEditors/CRE_EditorUIManager.h"

REGISTER_CLASS(CRE_UI_ClassList);

void CRE_UI_ClassList::DrawUI()
{
	ImGui::Begin(WindowTitle.c_str(), &bOpen);
	CRE_ObjectFactory& OF = CRE_ObjectFactory::Get();

	CRE_ClassBase* WantsToSpawn = nullptr;

	CRE_ClassBase* Base = OF.GetClass(CRE_ManagedObject::StaticClass());
	WantsToSpawn = ShowTable_Classes(Base);

	if (WantsToSpawn)
	{
		CRE_EditorUIManager& UIMan = CRE_EditorUIManager::Get();
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


void RecurseClass(CRE_ClassBase* Class)
{
	ImGui::PushID(Class);
	ImGui::Value("ClassID", Class->GetClassGUID().GetNumber());

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

void RecurseClass_Table(CRE_ClassBase* Class, CRE_ClassBase*& WantsToSpawn)
{
	ImGui::PushID(Class);
	//ImGui::SameLine();

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	ImGui::Text(Class->GetClassFriendlyName().c_str());

	ImGui::TableNextColumn();

	CRE_Styles::ButtonStyleStart();

	//Clicking the class name spawns the object.
	if (ImGui::Button(Class->GetClassFriendlyName().c_str()))
	{
		WantsToSpawn = Class;
	}

	CRE_Styles::ButtonStyleEnd();

	ImGui::Indent(20.f);

	for (CRE_ClassBase* Child : Class->GetChildren())
	{
		RecurseClass_Table(Child, WantsToSpawn);
	}

	ImGui::Unindent(20.f);

	ImGui::PopID();
}

CRE_ClassBase* CRE_UI_ClassList::ShowTable_Classes(CRE_ClassBase* Class)
{
	const int ColumnCount = 2;
	const char* ColumnNames[ColumnCount] = { "Class Name", "Create" };
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

		//float indent_step = (float)((int)ImGui::GetTextLineHeightWithSpacing() / 2);

		RecurseClass_Table(Class, WantsToSpawn);

		ImGui::EndTable();
	}

	return WantsToSpawn;
}

