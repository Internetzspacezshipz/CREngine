#include "CrUI_RenderableEditor.h"

//Edited type
#include "BasicObjects/CrRenderable.h"
#include "CrFieldEditor.inl"

//Styles
#include "UserInterface/CrUIStyles.h"

#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_RenderableEditor);

ADD_UI_EDITOR(CrRenderable, CrUI_RenderableEditor);

void CrUI_RenderableEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CrRenderable>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	bool bWasEdited = false;

	bWasEdited |= EditField<"Material">(Casted->Material);
	bWasEdited |= EditField<"MeshData">(Casted->Mesh);

	if (bWasEdited)
	{
		MarkAssetNeedsSave();
	}

	ImGui::End();
}
