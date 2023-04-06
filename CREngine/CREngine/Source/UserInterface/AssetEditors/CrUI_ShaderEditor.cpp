#include "CrUI_ShaderEditor.h"

//Edited type
#include "BasicObjects/CrShader.h"

//Styles
#include "UserInterface/CrUIStyles.h"
#include "CrFieldEditor.inl"

#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_ShaderEditor);

ADD_UI_EDITOR(CrShader, CrUI_ShaderEditor);

void CrUI_ShaderEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle, &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CrShader>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	bool bWasEdited = false;

	bWasEdited |= EditField<"Material", FolderLocation_Shaders, ".spv">(Casted->ImportPath);

	if (bWasEdited)
	{
		MarkAssetNeedsSave();
	}

	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	ImGui::End();
}
 