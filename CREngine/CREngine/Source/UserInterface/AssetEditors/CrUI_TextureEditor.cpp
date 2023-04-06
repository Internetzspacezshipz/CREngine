#include "CrUI_TextureEditor.h"

//Edited type
#include "BasicObjects/CrTexture.h"

//Styles
#include "UserInterface/CrUIStyles.h"

#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_TextureEditor);

ADD_UI_EDITOR(CrTexture, CrUI_TextureEditor);

void CrUI_TextureEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle.c_str(), &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CrTexture>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	bool bWasEdited = false;

	bWasEdited |= EditField<"Texture File", FolderLocation_Assets, ".png">(Casted->ImportPath);
	
	bWasEdited |= EditField<"Texture Format">(Casted->CompressionType);

	ImGui::Text("Size: %ix%i", Casted->TextureWidth, Casted->TextureHeight);

	ImGui::Text("Channels: %i", Casted->TextureChannels);

	ImGui::Text("Raw Size Bytes: %i", Casted->GetSizeBytes());

	ImGui::Text("Compressed Size Bytes: %i", Casted->GetSizeBytesReal());

	if (bWasEdited)
	{
		MarkAssetNeedsSave();
	}

	if (Casted->ValidData() && Casted->GetSizeBytes())
	{
		if (ImGui::CollapsingHeader("Show Image", DefaultCollapsingHeaderFlags))
		{
			float LargestSide = std::max(std::max((float)Casted->TextureWidth, (float)Casted->TextureHeight), 1.f);//added 1 here to make sure it can never div/zero

			float Scale = 500.f / LargestSide;

			//Shows actual texture size.

			//The size we want to zoom to.
			float my_tex_w_zoomed = (float)Casted->TextureWidth * Scale;
			float my_tex_h_zoomed = (float)Casted->TextureHeight * Scale;

			ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
			ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
			ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
			ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);

			ImGui::Image(Casted->GetData()->DescriptorSet, ImVec2(my_tex_w_zoomed, my_tex_h_zoomed), uv_min, uv_max, tint_col, border_col);
		}
	}
	else if (!Casted->ImportPath.empty())
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
