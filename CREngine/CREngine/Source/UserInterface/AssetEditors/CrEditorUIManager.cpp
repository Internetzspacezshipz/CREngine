#include "CrEditorUIManager.h"
#include <UserInterface/AssetEditors/CrUI_Editor_AssetBase.h>
#include <CrGlobals.h>
#include "CrApp.h"

CrEditorUIManager& CrEditorUIManager::Get()
{
	static CrEditorUIManager Manager;
	return Manager;
}

bool CrEditorUIManager::MakeEditUI(CrLoadable<CrManagedObject> Object)
{
	//Remove expired UI/assets
	RemoveByPredicate(ExistingEditors, [](const Pair<HWP<CrManagedObject>, WP<CrUI_Editor_AssetBase>>& Item)->bool
		{
			return Item.first.expired() || Item.second.expired();
		});

	if (Object.IsLoadedOrLoadable())
	{
		SP<CrManagedObject> ObjectActual = Object.Get<true>();
		//If we find that this asset is already being edited by an active window, then bring it to focus.
		auto Found = ExistingEditors.find(ObjectActual);
		if (Found != ExistingEditors.end())
		{
			//Window will be valid since we just removed all invalids at the start of the function.
			ImGui::SetWindowFocus(Found->second.lock()->WindowTitle.c_str());
			return true;
		}

		SP<CrUI_Editor_AssetBase> NewUI;
		ClassGUID EditUIClass = FindUIClass(ObjectActual->GetClass());

		if (EditUIClass.IsValidID())
		{
			NewUI = DCast<CrUI_Editor_AssetBase>(CrGlobals::GetAppPointer()->MakeUI(EditUIClass));
		}
		else
		{
			//Open default editor that allows only editing of filename.
			NewUI = DCast<CrUI_Editor_AssetBase>(CrGlobals::GetAppPointer()->MakeUI(CrUI_Editor_AssetBase::StaticClass()));
		}

		assert(NewUI);

		ExistingEditors.emplace(ObjectActual, NewUI);

		//Add the edited object to the UI.
		NewUI->SetEditedAsset(Object);

		return true;
	}
	return false;
}

bool CrEditorUIManager::HasEditUI(ClassGUID Class)
{
	return FindUIClass(Class).IsValidID();
}

ClassGUID CrEditorUIManager::FindUIClass(ClassGUID Class)
{
	auto Found = AssetToUIClass.find(Class);
	if (Found == AssetToUIClass.end())
	{
		return ClassGUID();
	}
	return Found->second;
}
