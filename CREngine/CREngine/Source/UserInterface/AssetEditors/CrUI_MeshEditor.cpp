#include "CrUI_MeshEditor.h"

//Edited type
#include "BasicObjects/CrMesh.h"

//Styles
#include "UserInterface/CrUIStyles.h"

#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_MeshEditor);

ADD_UI_EDITOR(CrMesh, CrUI_MeshEditor);

void CrUI_MeshEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CrMesh>();
	
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
