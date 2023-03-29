#include "CRE_UI_MeshEditor.h"

//Edited type
#include "BasicObjects/CRE_Mesh.h"

//Styles
#include "UserInterface/CRE_UIStyles.h"

#include "CRE_EditorUIManager.h"

REGISTER_CLASS(CRE_UI_MeshEditor);

ADD_UI_EDITOR(CRE_Mesh, CRE_UI_MeshEditor);

void CRE_UI_MeshEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CRE_Mesh>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	//TODO: Make macros for these pragmas.
#pragma warning(push)
#pragma warning(disable:4244)
	String Str(Casted->File.native().begin(), Casted->File.native().end());
#pragma warning(pop)

	if (ImGui::InputText("Path", &Str))
	{
		Casted->File = Str;
		MarkAssetNeedsSave();
	}

	ImGui::End();
}
