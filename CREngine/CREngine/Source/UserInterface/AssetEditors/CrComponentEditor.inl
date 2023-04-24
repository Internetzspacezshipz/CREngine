#pragma once

#include "CrComponent.h"
#include "CrEditorUIManager.h"
#include "CrUI_Editor_AssetBase.h"

#include "CrGlobals.h"

//Edits a component with an inline editor window inside the current window.
static bool EditComponent(CrComponentBase& Component, SP<CrUI_Editor_AssetBase> CurrentWindow)
{
	auto& UIMan = CrEditorUIManager::Get();
	auto ComponentObj = Component.Get();
	CrID Class = UIMan.FindUIClass(ComponentObj->GetClass());

	//Maybe later make this inline somehow.
	if (Class.IsValidID())
	{
		ImGui::SeparatorText(ComponentObj->GetID().GetStringPretty());

		SP<CrUI_Editor_AssetBase> EditorBase;

		EditorBase = DCast<CrUI_Editor_AssetBase>(FindByPredicate(CurrentWindow->Children,
			[&](const SP<CrUI_Base>& ItemB)->bool
			{
				if (SP<CrUI_Editor_AssetBase> Item = DCast<CrUI_Editor_AssetBase>(ItemB))
				{
					return Item->CurrentEditedAsset.Get() == ComponentObj;
				}
				return false;
			}));

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

static bool EditInstance(SP<CrManagedObject> Object, SP<CrUI_Editor_AssetBase> CurrentWindow)
{
	auto& UIMan = CrEditorUIManager::Get();
	CrID Class = UIMan.FindUIClass(Object->GetClass());

	//Maybe later make this inline somehow.
	if (Class.IsValidID())
	{
		ImGui::SeparatorText(Object->GetID().GetStringPretty());

		SP<CrUI_Editor_AssetBase> EditorBase;

		EditorBase = DCast<CrUI_Editor_AssetBase>(FindByPredicate(CurrentWindow->Children,
			[&](const SP<CrUI_Base>& Item)->bool
			{
				if (auto CItem = DCast<CrUI_Editor_AssetBase>(Item))
				{
					return CItem->CurrentEditedAsset.Get() == Object;
				}
				return false;
			}));

		if (EditorBase == nullptr)
		{
			EditorBase = UIMan.FindExisting(Object);

			if (EditorBase == nullptr)
			{
				EditorBase = UIMan.MakeSubEditUI(Object);

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
