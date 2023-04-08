#include "CrUI_SoundEditor.h"

//Edited type
#include "BasicObjects/CrSound.h"

//Styles
#include "UserInterface/CrUIStyles.h"

#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_SoundEditor);

ADD_UI_EDITOR(CrSound, CrUI_SoundEditor);

void CrUI_SoundEditor::DrawUI()
{
	Super::DrawUI();
	
	ImGui::Begin(WindowTitle, &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CrSound>();
	
	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	bool bWasEdited = false;

	bWasEdited |= EditField<"Sound File", FolderLocation_Assets, ".ogg">(Casted->ImportPath);

	ImGui::Checkbox("Looping", &Casted->Settings.bLooping);
	ImGui::SliderFloat("Volume", &Casted->Settings.Volume, 0.f, 1.f);



	if (bWasEdited)
	{
		MarkAssetNeedsSave();
	}

	if (!Casted->ImportPath.empty())
	{
		if (ImGui::Button("Play"))
		{
			Casted->PlayThrowaway();
		}
		ImGui::SameLine();
		if (ImGui::Button("Stop"))
		{

		}
		ImGui::SameLine();
		if (ImGui::Button("Load"))
		{
			Casted->LoadSound();
		}
	}

	ImGui::End();
}
