#pragma once
#include "UserInterface/CRE_UI_Base.h"
#include "CRE_Loadable.hpp"


class CRE_UI_Editor_AssetBase : public CRE_UI_Base
{
	DEF_CLASS(CRE_UI_Editor_AssetBase, CRE_UI_Base);

	virtual void Construct() override;

	virtual void DrawUI() override;

	virtual void RemoveUI(bool bPromptAllowed = false) override;

	//Call this when the asset has been edited.
	void MarkAssetNeedsSave() { bWantsSave = true; }
	void SaveAssetWithPrompts();

	//Flag for when the asset can be saved, but has not been yet.
	bool bWantsSave = false;
	bool bIsSaving = false;

	virtual ImGuiWindowFlags GetWindowFlags();

	void SetEditedAsset(CRE_Loadable<CRE_ManagedObject> NewAsset);

	void PopupUI();

	//Dcast to the expected asset type...
	template<typename AsType = CRE_ManagedObject>
	SP<AsType> GetEditedAsset() const
	{
		return DCast<AsType>(CurrentEditedAsset.Get<true>());
	}

	//Put this as private until further notice (who is noticing but me?)

private:

	//Variable for rename operation.
	String ActiveEditFilename;

	CRE_Loadable<CRE_ManagedObject> CurrentEditedAsset;
};

