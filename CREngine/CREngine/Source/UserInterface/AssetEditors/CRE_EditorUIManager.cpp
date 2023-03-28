#include "CRE_EditorUIManager.h"
#include <UserInterface/AssetEditors/CRE_UI_Editor_AssetBase.h>
#include <CRE_Globals.h>
#include "CRE_App.hpp"

CRE_EditorUIManager& CRE_EditorUIManager::Get()
{
	static CRE_EditorUIManager Manager;
	return Manager;
}

bool CRE_EditorUIManager::MakeEditUI(CRE_Loadable<CRE_ManagedObject> Object)
{
	//Remove expired UI/assets
	RemoveByPredicate(ExistingEditors, [](const Pair<HWP<CRE_ManagedObject>, WP<CRE_UI_Editor_AssetBase>>& Item)->bool
		{
			return Item.first.expired() || Item.second.expired();
		});

	if (Object.IsLoadedOrLoadable())
	{
		SP<CRE_ManagedObject> ObjectActual = Object.Get<true>();
		//If we find that this asset is already being edited by an active window, then bring it to focus.
		auto Found = ExistingEditors.find(ObjectActual);
		if (Found != ExistingEditors.end())
		{
			//Window will be valid since we just removed all invalids at the start of the function.
			ImGui::SetWindowFocus(Found->second.lock()->WindowTitle.c_str());
			return true;
		}

		SP<CRE_UI_Editor_AssetBase> NewUI;
		ClassGUID EditUIClass = FindUIClass(ObjectActual->GetClass());

		if (EditUIClass.IsValidID())
		{
			NewUI = DCast<CRE_UI_Editor_AssetBase>(CRE_Globals::GetAppPointer()->MakeUI(EditUIClass));
		}
		else
		{
			//Open default editor that allows only editing of filename.
			NewUI = DCast<CRE_UI_Editor_AssetBase>(CRE_Globals::GetAppPointer()->MakeUI(CRE_UI_Editor_AssetBase::StaticClass()));
		}

		assert(NewUI);

		ExistingEditors.emplace(ObjectActual, NewUI);

		//Add the edited object to the UI.
		NewUI->SetEditedAsset(Object);

		return true;
	}
	return false;
}

bool CRE_EditorUIManager::HasEditUI(ClassGUID Class)
{
	return FindUIClass(Class).IsValidID();
}

ClassGUID CRE_EditorUIManager::FindUIClass(ClassGUID Class)
{
	auto Found = AssetToUIClass.find(Class);
	if (Found == AssetToUIClass.end())
	{
		return ClassGUID();
	}
	return Found->second;
}
