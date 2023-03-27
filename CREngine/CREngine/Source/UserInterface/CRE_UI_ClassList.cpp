#include "CRE_UI_ClassList.h"
#include "CRE_UIStyles.h"

#include <CRE_Globals.h>
#include "CRE_App.hpp"

#include "AssetEditors/CRE_UI_Editor_AssetBase.h"

REGISTER_CLASS(CRE_UI_ClassList);

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

//Do copy to clipboard - maybe this could be another button.
//ImGui::LogToClipboard();
//ImGui::LogText(GUIDStr.c_str());
//ImGui::LogFinish();

void RecurseClass_Table(CRE_ClassBase* Class, CRE_ClassBase*& WantsToSpawn)
{
	ImGui::PushID(Class);
	//ImGui::SameLine();

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	//Clicking the class name spawns the object.
	if (ImGui::Button(Class->GetClassFriendlyName().c_str(), { 150.f, ImGui::GetTextLineHeightWithSpacing() }))
	{
		WantsToSpawn = Class;
	}

	ImGui::TableNextColumn();

	ImGui::Text(Class->GetClassFriendlyName().c_str());

	ImGui::Indent(20.f);

	for (CRE_ClassBase* Child : Class->GetChildren())
	{
		RecurseClass_Table(Child, WantsToSpawn);
	}

	ImGui::Unindent(20.f);

	ImGui::PopID();
}

void CRE_UI_ClassList::DrawUI()
{
	bool bStayOpen = true;

	ImGui::Begin("Class List", &bStayOpen);

	CRE_ObjectFactory& OF = CRE_ObjectFactory::Get();

	CRE_ClassBase* WantsToSpawn = nullptr;

	CRE_ClassBase* Base = OF.GetClass(CRE_ManagedObject::StaticClass());
	WantsToSpawn = ShowTable_Classes(Base);

	if (WantsToSpawn)
	{
		CRE_EditorUIManager& UIMan = CRE_EditorUIManager::Get();
		auto NewInstanceClass = WantsToSpawn->GetClassGUID();
		if (UIMan.HasEditUI(NewInstanceClass))
		{
			auto Out = OF.Create(NewInstanceClass);
			UIMan.MakeEditUI(Out);
		}
	}

	ImGui::End();

	if (!bStayOpen)
	{
		CRE_Globals::GetAppPointer()->RemoveUI(this);
	}
}

CRE_ClassBase* CRE_UI_ClassList::ShowTable_Classes(CRE_ClassBase* Class)
{
	const int column_count = 2;
	const char* column_names[column_count] = { "Class ID", "Class Name" };
	static ImGuiTableColumnFlags column_flags[column_count] = { ImGuiTableColumnFlags_DefaultSort, ImGuiTableColumnFlags_None };

	const ImGuiTableFlags flags
		= ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV
		| ImGuiTableFlags_Resizable | ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody;

	ImVec2 outer_size = ImVec2(0.0f, 0.f/*ImGui::GetTextLineHeightWithSpacing() * 30*/);

	CRE_ClassBase* WantsToSpawn = nullptr;

	if (ImGui::BeginTable("table_columns_flags", column_count, flags, outer_size))
	{
		for (int column = 0; column < column_count; column++)
		{
			ImGui::TableSetupColumn(column_names[column], column_flags[column]);
		}
		ImGui::TableHeadersRow();

		//float indent_step = (float)((int)ImGui::GetTextLineHeightWithSpacing() / 2);

		RecurseClass_Table(Class, WantsToSpawn);

		ImGui::EndTable();
	}

	return WantsToSpawn;
}

