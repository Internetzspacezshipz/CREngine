#include "CrUI_MaterialEditor.h"

#include "CrFieldEditor.inl"

//Edited type
#include "BasicObjects/CrMaterial.h"
#include "BasicObjects/CrTexture.h"

//Styles
#include "UserInterface/CrUIStyles.h"

#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_MaterialEditor);

ADD_UI_EDITOR(CrMaterial, CrUI_MaterialEditor);

void CrUI_MaterialEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle, &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CrMaterial>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}
	bool bWasEdited = false;

	bWasEdited |= EditField<"Vertex Shader">(Casted->VertexShader);
	bWasEdited |= EditField<"Fragment Shader">(Casted->FragmentShader);
	bWasEdited |= EditField<"Texture">(Casted->Texture);

	if (bWasEdited)
	{
		MarkAssetNeedsSave();
	}

	ImGui::End();
}
