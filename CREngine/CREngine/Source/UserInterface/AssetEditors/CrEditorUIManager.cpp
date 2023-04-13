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
	return MakeEditUI_Returned(Object) != nullptr;
}

SP<CrUI_Editor_AssetBase> CrEditorUIManager::MakeEditUI_Returned(CrLoadable<CrManagedObject> Object)
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
		auto Found = FindExisting(ObjectActual);
		if (Found)
		{
			ImGui::SetWindowFocus(Found->WindowTitle.c_str());
			return Found;
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

		//Add the edited object to the UI.
		NewUI->SetEditedAsset(ObjectActual);

		ExistingEditors.emplace(ObjectActual, NewUI);

		return NewUI;
	}
	return SP<CrUI_Editor_AssetBase>(nullptr);
}

SP<CrUI_Editor_AssetBase> CrEditorUIManager::MakeSubEditUI(SP<CrManagedObject> Existing)
{
	SP<CrUI_Editor_AssetBase> NewUI;
	ClassGUID EditUIClass = FindUIClass(Existing->GetClass());

	if (EditUIClass.IsValidID())
	{
		NewUI = DCast<CrUI_Editor_AssetBase>(CrGlobals::GetAppPointer()->MakeUINoAdd(EditUIClass));
	}
	else
	{
		//Open default editor that allows only editing of filename.
		NewUI = DCast<CrUI_Editor_AssetBase>(CrGlobals::GetAppPointer()->MakeUINoAdd(CrUI_Editor_AssetBase::StaticClass()));
	}

	assert(NewUI);

	//Add the edited object to the UI.
	NewUI->SetEditedAsset(Existing);

	return NewUI;
}

SP<CrUI_Editor_AssetBase> CrEditorUIManager::FindExisting(SP<CrManagedObject> Existing)
{
	RemoveByPredicate(ExistingEditors, [](const Pair<HWP<CrManagedObject>, WP<CrUI_Editor_AssetBase>>& Item)->bool
		{
			return Item.first.expired() || Item.second.expired();
		});

	auto Found = ExistingEditors.find(Existing);
	if (Found != ExistingEditors.end())
	{
		//Window will be valid since we just removed all invalids at the start of the function.
		return Found->second.lock();
	}

	return SP<CrUI_Editor_AssetBase>(nullptr);
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
