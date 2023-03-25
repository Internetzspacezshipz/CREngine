#include "CRE_UI_AssetListEditor.hpp"
#include "CRE_Globals.hpp"
#include "CRE_App.hpp"
#include "CRE_AssetList.hpp"

#include "BasicObjects/CRE_Texture.hpp"
#include "BasicObjects/CRE_Mesh.hpp"

REGISTER_CLASS(CRE_UI_AssetListEditor, CRE_UI_Base);

//Remove keybind here.
CRE_UI_AssetListEditor::~CRE_UI_AssetListEditor()
{
	OpenKeyBind.lock()->Remove();
}

static ImVec2 DefaultButtonSize{50.f, 20.f};
static ImGuiTreeNodeFlags DefaultCollapsingHeaderFlags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnArrow;

struct CRE_EditorUIManager
{
	typedef fu2::function<void(CRE_ManagedObject*)> UIFunc;
	typedef fu2::function<void(CRE_ManagedObject*)>* UIFunc_p;

	static CRE_EditorUIManager& Get()
	{
		static CRE_EditorUIManager Manager;
		return Manager;
	}

	std::unordered_map<ClassGUID, UIFunc> AssetToUIFunc;

	void CallEditUI(CRE_ManagedObject* Object)
	{
		auto Found = AssetToUIFunc.find(Object->GetClass());
		if (Found == AssetToUIFunc.end())
		{
			return;
		}
		Found->second(Object);
	}

	bool HasEditUI(CRE_ManagedObject* Object)
	{
		auto Found = AssetToUIFunc.find(Object->GetClass());
		if (Found == AssetToUIFunc.end())
		{
			return false;
		}
		return true;
	}
};

struct RegEditorUIFunc
{
	RegEditorUIFunc(ClassGUID ClassID, CRE_EditorUIManager::UIFunc InFunc)
	{
		CRE_EditorUIManager::Get().AssetToUIFunc.emplace(ClassID, InFunc);
	}
};

static RegEditorUIFunc TextureEdit(CRE_Texture::StaticClass(),
[](CRE_ManagedObject* Object)
{
	CRE_Texture* Casted = DCast<CRE_Texture>(Object);
//TODO: Make macros for these pragmas.
#pragma warning(push)
#pragma warning(disable:4244)
	std::string Str(Casted->File.native().begin(), Casted->File.native().end());
#pragma warning(pop)

	ImGui::Indent(20.f);

	if (ImGui::InputText("Path", &Str))
	{
		Casted->File = Str;
	}

	if (Texture* Tex = Casted->GetTextureActual())
	{
		if (ImGui::CollapsingHeader("ShowImage", DefaultCollapsingHeaderFlags))
		{
			float LargestSide = std::max(std::max((float)Tex->image.texWidth, (float)Tex->image.texHeight), 1.f);//added 1 here to make sure it can never div/zero

			float Scale = 500.f/LargestSide;

			//Shows actual texture size.
			ImGui::Text("Size: %.0fx%.0f", (float)Tex->image.texWidth, (float)Tex->image.texHeight);

			//The size we want to zoom to.
			float my_tex_w_zoomed = (float)Tex->image.texWidth * Scale;
			float my_tex_h_zoomed = (float)Tex->image.texHeight * Scale;

			ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
			ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

			ImGui::Image(Tex->DescriptorSet, ImVec2(my_tex_w_zoomed, my_tex_h_zoomed), uv_min, uv_max, tint_col, border_col);
		}
	}
	ImGui::Unindent(20.f);
});

static RegEditorUIFunc MeshEdit(CRE_Mesh::StaticClass(),
[](CRE_ManagedObject* Object)
{
	CRE_Mesh* Casted = DCast<CRE_Mesh>(Object);

	std::string Str(Casted->File.native().begin(), Casted->File.native().end());

	ImGui::Indent(20.f);

	if (ImGui::InputText("Path", &Str))
	{
		Casted->File = Str;
	}
});

/*
static RegEditorUIFunc MeshEdit(CRE_AssetList::StaticClass(),
[](CRE_ManagedObject* Object)
{
	CRE_AssetList* Casted = DCast<CRE_AssetList>(Object);

	std::string Str(Casted->File.native().begin(), Casted->File.native().end());

	ImGui::Indent(20.f);

	if (ImGui::InputText("Path", &Str))
	{
		Casted->File = Str;
	}
});
*/

//Returns true to ask for deletion.
bool ShowObjectInfo(CRE_ManagedObject* Object)
{
	if (!Object)
	{
		return true;
	}
	bool bShouldDelete = false;

	ImGui::PushID(Object);
	ImGui::Separator();

	std::string StringName = Object->GetId().GetString();
	if (ImGui::InputText("Object Instance", &StringName, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	{
		Object->Rename(StringName);
	}

	ImGui::Value("ClassID", Object->GetClass());
	auto* CO = Object->GetClassObj();

	ImGui::Text(CO->GetClassFriendlyName().c_str());
	ImGui::SameLine();

	if (ImGui::Button("Delete Object", DefaultButtonSize))
	{
		bShouldDelete = true;
	}

	CRE_EditorUIManager& EditUIMan = CRE_EditorUIManager::Get();

	if (EditUIMan.HasEditUI(Object))
	{
		if (ImGui::CollapsingHeader("Edit Object", DefaultCollapsingHeaderFlags))
		{
			ImGui::PushID(Object->GetClass());
			EditUIMan.CallEditUI(Object);
			ImGui::PopID();
		}
	}

	ImGui::Separator();

	ImGui::PopID();
	return bShouldDelete;
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

void RecurseClass_Table(CRE_ClassBase* Class, CRE_ClassBase*& WantsToSpawn)
{
	ImGui::PushID(Class);
	//ImGui::SameLine();

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	//Clicking the class name spawns the object.
	if (ImGui::Button(Class->GetClassFriendlyName().c_str(), {90.f, ImGui::GetTextLineHeightWithSpacing()}))
	{
		WantsToSpawn = Class;
		//Do copy to clipboard - maybe this could be another button.
		//ImGui::LogToClipboard();
		//ImGui::LogText(GUIDStr.c_str());
		//ImGui::LogFinish();
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

CRE_ClassBase* CRE_UI_AssetListEditor::ShowTable_Classes(CRE_ClassBase* Class)
{
	const int column_count = 2;
	const char* column_names[column_count] = { "Class ID", "Class Name" };
	static ImGuiTableColumnFlags column_flags[column_count] = { ImGuiTableColumnFlags_DefaultSort, ImGuiTableColumnFlags_None };

	const ImGuiTableFlags flags
		= ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_ScrollY
		| ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV
		| ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_Sortable;

	ImVec2 outer_size = ImVec2(0.0f, ImGui::GetTextLineHeightWithSpacing() * 9);

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

void CRE_UI_AssetListEditor::DrawUI()
{
	if (bIsOpen)
	{
		CRE_Serialization& Serialization = CRE_Serialization::Get();
		CRE_App* App = CRE_Globals::GetAppPointer();
		CRE_ObjectFactory& OF = CRE_ObjectFactory::Get();


		SP<CRE_AssetList> PinnedAssetList = DCast<CRE_AssetList>(App->GetRootAssetList());

		if (PinnedAssetList == nullptr)
		{
			return;
		}

		

		std::string FileName = PinnedAssetList->AssetListPath.filename().generic_string();

		ImGui::Begin("AssetListEditor", &bIsOpen);

		//Disallow renaming of main asset.
		//if (CurrentAssetList != App->GetRootAssetList())
		{
			if (ImGui::InputText("Filename:", &FileName, ImGuiInputTextFlags_EnterReturnsTrue))
			{
				PinnedAssetList->AssetListPath = FileName;
			}
		}

		if (ImGui::Button("Save", DefaultButtonSize))
		{
			Serialization.Save(PinnedAssetList);
		}

		ImGui::SameLine();

		if (ImGui::Button("Load", DefaultButtonSize))
		{
			auto Shared = DCast<CRE_Obj>(PinnedAssetList);
			Serialization.Reload(Shared, PinnedAssetList->GetId());
		}

		ImGui::Checkbox("IMGUI_DEMO", &bOpenDemo);
		if (bOpenDemo)
		{
			ImGui::ShowDemoWindow(&bOpenDemo);
		}

		CRE_ClassBase* WantsToSpawn = nullptr;

		if (ImGui::CollapsingHeader("List Asset", DefaultCollapsingHeaderFlags))
		{
			CRE_ClassBase* Base = OF.GetClass(CRE_Obj::StaticClass());
			WantsToSpawn = ShowTable_Classes(Base);
		}


		if (WantsToSpawn)
		{
			PinnedAssetList->Objects.emplace_back(OF.Create(WantsToSpawn->GetClassGUID()));
		}


		//FilterExisting.Draw("Filter existing objects");

		auto it = PinnedAssetList->Objects.begin();
		while (it != PinnedAssetList->Objects.end())
		{
			Object_sp Ob = (*it).Get();

			//if (!Ob) continue;

			//if (FilterExisting.PassFilter(Ob->GetId().GetString().c_str()))
			{
				if (ShowObjectInfo(Ob.get()))
				{
					it = PinnedAssetList->Objects.erase(it);
				}
				else
				{
					it++;
				}
			}
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
