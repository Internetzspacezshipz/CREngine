#include "CrUI_SoundPlayerEditor.h"
#include <BasicObjects/CrSoundPlayer.h>
#include "CrEditorUIManager.h"

REGISTER_CLASS(CrUI_SoundPlayerEditor);

ADD_UI_EDITOR(CrSoundPlayer, CrUI_SoundPlayerEditor);


void CrUI_SoundPlayerEditor::DrawUI()
{
	Super::DrawUI();

	ImGui::Begin(WindowTitle, &bOpen, GetWindowFlags());

	auto Casted = GetEditedAsset<CrSoundPlayer>();

	if (Casted.get() == nullptr)
	{
		ImGui::End();
		return;
	}

	bool bWasEdited = false;

	bWasEdited |= EditField<"Played Sound">(Casted->Sound);
	
	if (bWasEdited)
	{
		MarkAssetNeedsSave();
	}

	ImGui::End();
}
