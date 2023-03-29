#include "CRE_UI_ShaderEditor.h"

//Edited type
#include "BasicObjects/CRE_Shader.h"

//Styles
#include "UserInterface/CRE_UIStyles.h"

#include "CRE_EditorUIManager.h"

REGISTER_CLASS(CRE_UI_ShaderEditor);

ADD_UI_EDITOR(CRE_Shader, CRE_UI_ShaderEditor);

void CRE_UI_ShaderEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CRE_Shader>();
	
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
