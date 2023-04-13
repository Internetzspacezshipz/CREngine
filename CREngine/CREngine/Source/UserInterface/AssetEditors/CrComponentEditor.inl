#pragma once

#include "CrComponent.h"
#include "CrEditorUIManager.h"
#include "CrUI_Editor_AssetBase.h"

//Edits a component with an inline editor window inside the current window.
bool EditComponent(CrComponentBase& Component, SP<CrUI_Editor_AssetBase> CurrentWindow)
{
	auto& UIMan = CrEditorUIManager::Get();
	auto ComponentObj = Component.Get();
	CrID Class = UIMan.FindUIClass(ComponentObj->GetClass());

	//Maybe later make this inline somehow.
	if (Class.IsValidID())
	{
		ImGui::SeparatorText(ComponentObj->GetID().GetStringPretty());

		SP<CrUI_Editor_AssetBase> EditorBase;

		EditorBase = FindByPredicate(CurrentWindow->Children, 
			[&](const SP<CrUI_Editor_AssetBase>& Item)->bool
			{
				return Item->CurrentEditedAsset.Get() == ComponentObj;
			});

		if (EditorBase == nullptr)
		{
			EditorBase = UIMan.FindExisting(Component.Get());

			if (EditorBase == nullptr)
			{
				EditorBase = UIMan.MakeSubEditUI(Component.Get());

				//Make one if we don't have one already.
				CurrentWindow->Children.push_back(EditorBase);
				EditorBase->Parent = CurrentWindow;

				//Reuse exact window name to ensure that the window will be inside the other.
				EditorBase->WindowTitle = CurrentWindow->WindowTitle;
			}
		}

		if (EditorBase != nullptr)
		{
			//the item must draw UI within the outer object.
			EditorBase->DrawUI();
			return EditorBase->bWantsSave;
		}

		ImGui::Separator();
	}
	return false;
}