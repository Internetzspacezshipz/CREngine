#include "CRE_UI_MaterialEditor.h"

//Edited type
#include "BasicObjects/CRE_Material.h"

//Styles
#include "UserInterface/CRE_UIStyles.h"

#include "CRE_EditorUIManager.h"

REGISTER_CLASS(CRE_UI_MaterialEditor);

ADD_UI_EDITOR(CRE_Material, CRE_UI_MaterialEditor);

void CRE_UI_MaterialEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CRE_Material>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	String VertShaderPath = Casted->VertexShader.GetID().GetString();
	if (ImGui::InputText("VertexShader", &VertShaderPath))
	{
		Casted->VertexShader.Set(VertShaderPath);
		MarkAssetNeedsSave();
	}

	String FragShaderPath = Casted->FragmentShader.GetID().GetString();
	if (ImGui::InputText("FragmentShader", &FragShaderPath))
	{
		Casted->FragmentShader.Set(FragShaderPath);
		MarkAssetNeedsSave();
	}

	ImGui::End();
}
