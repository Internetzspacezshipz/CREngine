#include "CRE_UI_TextureEditor.h"

//Edited type
#include "BasicObjects/CRE_Texture.h"

//Styles
#include "UserInterface/CRE_UIStyles.h"

#include "CRE_EditorUIManager.h"

REGISTER_CLASS(CRE_UI_TextureEditor);

ADD_UI_EDITOR(CRE_Texture, CRE_UI_TextureEditor);

void CRE_UI_TextureEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CRE_Texture>();
	
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

	if (Casted->ValidData())
	{
		Texture* Tex = Casted->GetData();
		if (ImGui::CollapsingHeader("Show Image", DefaultCollapsingHeaderFlags))
		{
			float LargestSide = std::max(std::max((float)Tex->image.texWidth, (float)Tex->image.texHeight), 1.f);//added 1 here to make sure it can never div/zero

			float Scale = 500.f / LargestSide;

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
	else
	{
		if (ImGui::Button("Load"))
		{
			if (!Casted->UploadTexture())
			{
				ImGui::OpenPopup("Failed to load texture");
			}
		}
	}

	if (ImGui::BeginPopupModal("Failed to load texture"))
	{
		if (ImGui::Button("Okay..."))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}			

	ImGui::End();
}
