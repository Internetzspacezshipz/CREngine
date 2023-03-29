#include "CRE_UI_RenderableEditor.h"

//Edited type
#include "BasicObjects/CRE_Renderable.h"

//Styles
#include "UserInterface/CRE_UIStyles.h"

#include "CRE_EditorUIManager.h"

REGISTER_CLASS(CRE_UI_RenderableEditor);

ADD_UI_EDITOR(CRE_Renderable, CRE_UI_RenderableEditor);

void CRE_UI_RenderableEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CRE_Renderable>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	String MaterialPath = Casted->MaterialOb.GetID().GetString();
	if (ImGui::InputText("Material", &MaterialPath))
	{
		Casted->MaterialOb.Set(MaterialPath);
		MarkAssetNeedsSave();
	}

	String MeshObPath = Casted->MeshOb.GetID().GetString();
	if (ImGui::InputText("FragmentShader", &MeshObPath))
	{
		Casted->MeshOb.Set(MeshObPath);
		MarkAssetNeedsSave();
	}

	ImGui::End();
}
